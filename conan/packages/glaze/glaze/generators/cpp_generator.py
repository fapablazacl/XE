from typing import Dict, List, Optional, Tuple

from glaze.generators.base import Generator
from glaze.model import Registry, Command, CommandParam, Enum
from glaze.utils.string_utils import split_capitalized


def _to_handle_name(class_str: str) -> str:
    """Convert a GL object class string to a CamelCase type name.

    e.g. 'texture' -> 'Texture', 'program pipeline' -> 'ProgramPipeline'
    """
    return "".join(word.capitalize() for word in class_str.split())


def _is_uint_handle(param: CommandParam) -> bool:
    """Return True if param's base GL type is GLuint (a potential object handle)."""
    return param.type == "GLuint"


def _is_string_return_command(command: Command) -> bool:
    """Return True if the command returns a C string (const GLubyte* or const GLchar*)."""
    rt = command.return_type
    return rt.is_const and rt.is_pointer and rt.name in ("GLubyte", "GLchar")


def _find_string_output_param(command: Command) -> Optional[CommandParam]:
    """Return the GLchar* non-const output param whose len= references another param, or None."""
    param_names = {p.name for p in command.params}
    for param in command.params:
        if (param.type == "GLchar" and param.is_pointer and not param.is_const
                and param.len and param.len in param_names):
            return param
    return None


def _find_length_param(command: Command) -> Optional[CommandParam]:
    """Return the GLsizei* length-output param (len='1'), or None."""
    for param in command.params:
        if (param.type == "GLsizei" and param.is_pointer
                and not param.is_const and param.len == "1"):
            return param
    return None


# ── Vendor-suffix stripping for group names ───────────────────────────────────

# Known vendor/extension suffixes that appear at the end of XML group names.
# Ordered longest-first so e.g. "MESA" is tried before any single-letter suffix.
_VENDOR_SUFFIXES = (
    "MESA", "INTEL", "APPLE", "SGIS", "SGIX", "SUNX", "QCOM", "3DFX",
    "INGR", "REND", "WIN", "ARB", "EXT", "NVX", "KHR", "OES", "AMD",
    "ATI", "IBM", "SUN", "NV", "HP", "IMG", "VIV", "DMP", "FJ",
)


def _build_group_rename(group_set: set) -> Dict[str, str]:
    """Return a mapping old_name → clean_name for every group whose suffix can be stripped
    without colliding with another group already in group_set."""
    rename: Dict[str, str] = {}
    for name in group_set:
        for suffix in _VENDOR_SUFFIXES:
            if name.endswith(suffix) and len(name) > len(suffix):
                clean = name[:-len(suffix)]
                # Only rename if the clean name is not already taken by another group
                if clean not in group_set:
                    rename[name] = clean
                break  # try only the first matching suffix
    return rename


# ── Location-type detection (name-based, no XML annotation available) ─────────

_UNIFORM_LOCATION_RETURN: frozenset = frozenset({
    "glGetUniformLocation",
    "glGetFragDataLocation",
    "glGetFragDataIndex",
})

_ATTRIB_LOCATION_RETURN: frozenset = frozenset({
    "glGetAttribLocation",
})


def _is_uniform_location_param(command: Command, param: CommandParam) -> bool:
    """True if this GLint 'location' param is a uniform location."""
    return (param.type == "GLint" and param.name == "location"
            and (command.name.startswith("glUniform")
                 or command.name.startswith("glProgramUniform")))


def _is_attrib_location_param(command: Command, param: CommandParam) -> bool:
    """True if this GLuint 'index' param is a vertex-attrib location."""
    return (param.type == "GLuint" and param.name == "index"
            and ("VertexAttrib" in command.name
                 or command.name == "glBindAttribLocation"))


class _Capitalizer:
    _EXCLUDED = {"1D", "2D", "3D"}

    def capitalize(self, value: str) -> str:
        if value in self._EXCLUDED:
            return value
        return value.capitalize()


class _EnumIdentifierConverter:
    """Converts a GL_ constant name into an 'e'-prefixed C++ enum entry."""

    def __init__(self, enum_name: str, capitalizer: _Capitalizer):
        self._enum_name = enum_name
        self._enum_name_parts = split_capitalized(enum_name)
        self._capitalizer = capitalizer

    def convert(self, constant: str) -> str:
        parts = constant.replace("GL_", "").split("_")
        parts = [self._capitalizer.capitalize(p) for p in parts]

        filtered = [p for p in parts if p not in self._enum_name_parts]
        if filtered:
            parts = filtered

        return "e" + "".join(parts)


class CppGenerator(Generator):
    """Generates a C++11 header with type-safe enum classes and inline wrappers.

    The generated header depends on the C-generated <glaze/gl.h> rather than
    GLAD, so it wraps the dynamic-loading function pointers from the C bindings.

    Output file:
      {api}.hpp
    """

    def __init__(self, registry: Registry):
        super().__init__(registry)
        self._capitalizer = _Capitalizer()
        self._handle_classes: Dict[str, str] = {}  # class_ string → CamelCase handle name
        self._group_rename: Dict[str, str] = {}     # xml group name → clean C++ type name

    @property
    def name(self) -> str:
        return "cpp"

    def generate(self, api: str, version: str) -> Dict[str, str]:
        self._check_api_version(api, version)
        consolidated = self.registry.consolidate(api, version)

        # Single pass: collect handle classes and enum groups from all included commands
        self._handle_classes = {}
        group_set: set = set()
        for command_name in consolidated.commands:
            command = self.registry.command_by_name.get(command_name)
            if command is None:
                continue
            for param in command.params:
                if param.class_ and _is_uint_handle(param):
                    self._handle_classes[param.class_] = _to_handle_name(param.class_)
                if param.has_group() and param.group in self.registry.group_to_enums:
                    group_set.add(param.group)

        # Strip vendor suffixes from group names (e.g. BufferTargetARB → BufferTarget)
        self._group_rename = _build_group_rename(group_set)

        # Resolve name conflicts between handle types and enum class names
        # Check against both original and cleaned group names
        clean_group_names = set(self._group_rename.values()) | (group_set - set(self._group_rename))
        for cls, name in list(self._handle_classes.items()):
            if name in clean_group_names:
                self._handle_classes[cls] = name + "Id"

        handle_types: List[Tuple[str, str]] = [
            (n, n) for n in sorted(set(self._handle_classes.values()))
        ]

        # Build enum class context — only include enums in the consolidated set
        enum_classes = []
        for group_name in sorted(group_set):
            all_enums = self.registry.group_to_enums[group_name]
            filtered = [e for e in all_enums if e.name in consolidated.enums]
            if filtered:
                enum_classes.append(self._enum_class_context(group_name, filtered))

        # Collect location types used by included commands
        need_uniform_location = False
        need_attrib_location = False
        for command_name in consolidated.commands:
            cmd = self.registry.command_by_name.get(command_name)
            if cmd is None:
                continue
            if cmd.name in _UNIFORM_LOCATION_RETURN or any(
                    _is_uniform_location_param(cmd, p) for p in cmd.params):
                need_uniform_location = True
            if cmd.name in _ATTRIB_LOCATION_RETURN or any(
                    _is_attrib_location_param(cmd, p) for p in cmd.params):
                need_attrib_location = True

        location_types: List[Tuple[str, str]] = []
        if need_attrib_location:
            location_types.append(("AttribLocation", "AttribLocation"))
        if need_uniform_location:
            location_types.append(("UniformLocation", "UniformLocation"))

        # Build inline function context
        functions = []
        for command_name in sorted(consolidated.commands):
            command = self.registry.command_by_name.get(command_name)
            if command is None:
                continue
            functions.append(self._function_context(command))
            # For Pattern B (void + GLchar* output buffer), add a std::string overload
            string_param = _find_string_output_param(command)
            if string_param:
                length_param = _find_length_param(command)
                functions.append(self._string_overload_context(command, string_param, length_param))

        filename = f"include/glaze/{api}.hpp"
        context = {
            "api": api,
            "handle_types": handle_types,
            "location_types": location_types,
            "enum_classes": enum_classes,
            "functions": functions,
        }
        return {filename: self._render_template("cpp/gl.hpp.j2", context)}

    # ----------------------------------------------------------------- checks

    def _check_api_version(self, api: str, version: str):
        available = self.registry.available_apis()
        if api not in available:
            raise ValueError(f"API '{api}' not found. Available: {list(available.keys())}")
        if version not in available[api]:
            raise ValueError(f"Version '{version}' not found for '{api}'. Available: {available[api]}")

    # --------------------------------------------------------------- contexts

    def _enum_class_context(self, group_name: str, enums: List[Enum]) -> dict:
        clean_name = self._group_rename.get(group_name, group_name)
        base_type = "GLboolean" if clean_name == "Boolean" else "GLenum"
        converter = _EnumIdentifierConverter(clean_name, self._capitalizer)
        entries = [{"name": converter.convert(e.name), "value": e.name} for e in enums]
        return {"group_name": clean_name, "base_type": base_type, "entries": entries}

    def _function_context(self, command: Command) -> dict:
        return_type_str = command.return_type_str or command.return_type.to_c_string()
        func_name = self._convert_function_name(command.name)
        params_str = ", ".join(self._generate_param_decl(p, command) for p in command.params)
        call_args_str = ", ".join(self._generate_call_arg(p, command) for p in command.params)

        # Pattern A: const GLubyte* / const GLchar* returns → wrap to std::string
        if _is_string_return_command(command):
            raw_type = command.return_type.name
            body = (
                f"    const {raw_type}* raw = {command.name}({call_args_str});\n"
                "    return raw ? reinterpret_cast<const char*>(raw) : std::string{};"
            )
            return {
                "return_type": "std::string",
                "func_name": func_name,
                "params_str": params_str,
                "gl_name": command.name,
                "body": body,
            }

        # Location return types
        if command.name in _UNIFORM_LOCATION_RETURN:
            body = f"    return UniformLocation({command.name}({call_args_str}));"
            return {"return_type": "UniformLocation", "func_name": func_name,
                    "params_str": params_str, "gl_name": command.name, "body": body}

        if command.name in _ATTRIB_LOCATION_RETURN:
            body = f"    return AttribLocation({command.name}({call_args_str}));"
            return {"return_type": "AttribLocation", "func_name": func_name,
                    "params_str": params_str, "gl_name": command.name, "body": body}

        return {
            "return_type": return_type_str,
            "func_name": func_name,
            "params_str": params_str,
            "call_args_str": call_args_str,
            "gl_name": command.name,
            "body": None,
        }

    def _string_overload_context(self, command: Command,
                                  string_param: CommandParam,
                                  length_param: Optional[CommandParam]) -> dict:
        """Build a std::string-returning overload for Pattern B string-output commands."""
        func_name = self._convert_function_name(command.name)

        # Overload params: all original params except the GLchar* buffer and GLsizei* length
        overload_params = [p for p in command.params
                           if p is not string_param and p is not length_param]
        params_str = ", ".join(self._generate_param_decl(p, command) for p in overload_params)

        # Build call args for the underlying C function, substituting the dropped params
        call_parts = []
        for p in command.params:
            if p is string_param:
                call_parts.append("&result[0]")
            elif p is length_param:
                call_parts.append("&length")
            else:
                call_parts.append(self._generate_call_arg(p, command))
        call_args_str = ", ".join(call_parts)

        body = (
            f"    std::string result(static_cast<std::size_t>({string_param.len}), '\\0');\n"
            "    GLsizei length = 0;\n"
            f"    {command.name}({call_args_str});\n"
            "    result.resize(static_cast<std::size_t>(length));\n"
            "    return result;"
        )
        return {
            "return_type": "std::string",
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
        }

    # ----------------------------------------------------------- param helpers

    def _convert_function_name(self, gl_name: str) -> str:
        # glClearColor → clearColor
        return gl_name[2:3].lower() + gl_name[3:]

    def _generate_param_decl(self, param: CommandParam,
                              command: Optional[Command] = None) -> str:
        return f"{self._param_type_str(param, command=command)} {param.name}"

    def _param_type_str(self, param: CommandParam, ignore_group: bool = False,
                        command: Optional[Command] = None) -> str:
        """Reconstruct the parameter type string, substituting enum class or handle name if applicable."""
        # Location type substitution (name-based heuristic)
        if command:
            if _is_uniform_location_param(command, param):
                return "UniformLocation"
            if _is_attrib_location_param(command, param):
                return "AttribLocation"

        # Strong handle substitution: GLuint → Texture / Buffer / etc.
        if param.class_ and _is_uint_handle(param) and param.class_ in self._handle_classes:
            handle_name = self._handle_classes[param.class_]
            parts = []
            for part in param.type_parts:
                if part == "GLuint":
                    parts.append(handle_name)
                elif part:
                    parts.append(part)
            return " ".join(parts)

        use_type = param.type
        if (
            not ignore_group
            and param.has_group()
            and param.group in self.registry.group_to_enums
        ):
            use_type = self._group_rename.get(param.group, param.group)

        parts = []
        for part in param.type_parts:
            if part == param.type:
                parts.append(use_type if use_type else part)
            elif part:
                parts.append(part)
        return " ".join(parts)

    def _generate_call_arg(self, param: CommandParam,
                            command: Optional[Command] = None) -> str:
        """Generate the argument expression for the call to the underlying GL function."""
        # Location types: extract .loc; attrib locations need a cast to GLuint.
        if command:
            if _is_uniform_location_param(command, param):
                return f"{param.name}.loc"
            if _is_attrib_location_param(command, param):
                return f"static_cast<GLuint>({param.name}.loc)"

        # Strong handle types: extract .id for value params; reinterpret_cast for pointer params.
        # Handle<Tag> is standard-layout (single GLuint member), so the cast is well-defined.
        if param.class_ and _is_uint_handle(param) and param.class_ in self._handle_classes:
            if param.is_pointer:
                const_str = "const " if param.is_const else ""
                return f"reinterpret_cast<{const_str}GLuint*>({param.name})"
            return f"{param.name}.id"

        if param.has_group() and param.group == "Boolean" and param.is_pointer:
            raw_type_str = self._param_type_str(param, ignore_group=True)
            return f"reinterpret_cast<{raw_type_str}>({param.name})"

        if param.has_group() and not param.is_pointer:
            raw_type = param.type
            return f"static_cast<{raw_type}>({param.name})"

        if self._type_must_change(param) and param.is_pointer:
            const_str = "const " if param.is_const else ""
            return f"reinterpret_cast<{const_str}{param.type}*>({param.name})"

        return param.name

    def _type_must_change(self, param: CommandParam) -> bool:
        return (
            param.has_group()
            and param.group in self.registry.group_to_enums
        )
