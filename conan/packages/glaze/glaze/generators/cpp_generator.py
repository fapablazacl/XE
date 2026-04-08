from typing import ClassVar

from glaze.doc_parser import FunctionDoc
from glaze.generators.base import Generator
from glaze.model import Command, CommandParam, ConsolidatedRequire, Enum, Registry
from glaze.utils.string_utils import split_capitalized

_CPP_KEYWORDS: frozenset = frozenset({
    "delete", "new", "class", "template", "operator", "return",
    "switch", "case", "default", "break", "continue", "if", "else",
    "for", "while", "do", "void", "int", "float", "double", "bool",
    "char", "namespace", "using", "static", "const", "virtual",
    "public", "private", "protected", "struct", "enum", "union",
    "typedef", "extern", "inline", "volatile", "register", "auto",
    "throw", "try", "catch", "this", "sizeof", "true", "false",
})


def _to_handle_name(class_str: str) -> str:
    """Convert a GL object class string to a CamelCase type name.

    e.g. 'texture' -> 'Texture', 'program pipeline' -> 'ProgramPipeline'
    """
    return "".join(word.capitalize() for word in class_str.split())


def _functor_struct_name(func_name: str) -> str:
    """Return the bare C++ functor struct name (without leading underscore).

    Mirrors the convention in `_build_functors`: '<UpperCamel>Fn'. The leading
    underscore is added by the Jinja template.
    e.g. 'genBuffer' -> 'GenBufferFn', 'createProgram' -> 'CreateProgramFn'.
    """
    return f"{func_name[0].upper()}{func_name[1:]}Fn"


def _is_uint_handle(param: CommandParam) -> bool:
    """Return True if param's base GL type is GLuint (a potential object handle)."""
    return param.type == "GLuint"


def _is_string_return_command(command: Command) -> bool:
    """Return True if the command returns a C string (const GLubyte* or const GLchar*)."""
    rt = command.return_type
    return rt.is_const and rt.is_pointer and rt.name in ("GLubyte", "GLchar")


def _find_data_upload_params(
    command: Command,
) -> tuple[CommandParam, CommandParam] | None:
    """Return (data_param, size_param) if command has a const void* data param with len referencing a size param."""
    param_by_name = {p.name: p for p in command.params}
    for param in command.params:
        if (
            param.is_void
            and param.is_pointer
            and param.is_const
            and param.len
            and param.len in param_by_name
        ):
            size_param = param_by_name[param.len]
            if size_param.type in ("GLsizei", "GLsizeiptr") and not size_param.is_pointer:
                return param, size_param
    return None


def _find_object_deletion_params(
    command: Command,
) -> tuple[CommandParam, CommandParam] | None:
    """Return (count_param, input_param) if command is a multi-object Delete pattern."""
    if not command.name.startswith("glDelete"):
        return None
    count_param = None
    input_param = None
    for param in command.params:
        if param.type == "GLsizei" and not param.is_pointer:
            count_param = param
        if (
            param.type == "GLuint"
            and param.is_pointer
            and param.is_const
            and param.class_
        ):
            input_param = param
    if count_param and input_param:
        return count_param, input_param
    return None


def _find_object_creation_params(
    command: Command,
) -> tuple[CommandParam, CommandParam] | None:
    """Return (count_param, output_param) if command is a Gen/Create pattern."""
    if not (command.name.startswith("glGen") or command.name.startswith("glCreate")):
        return None
    count_param = None
    output_param = None
    for param in command.params:
        if param.type == "GLsizei" and not param.is_pointer:
            count_param = param
        if (
            param.type == "GLuint"
            and param.is_pointer
            and not param.is_const
            and param.class_
        ):
            output_param = param
    if count_param and output_param:
        return count_param, output_param
    return None


def _find_string_output_param(command: Command) -> CommandParam | None:
    """Return the GLchar* non-const output param whose len= references another param, or None."""
    param_names = {p.name for p in command.params}
    for param in command.params:
        if (
            param.type == "GLchar"
            and param.is_pointer
            and not param.is_const
            and param.len
            and param.len in param_names
        ):
            return param
    return None


def _find_length_param(command: Command) -> CommandParam | None:
    """Return the GLsizei* length-output param (len='1'), or None."""
    for param in command.params:
        if param.type == "GLsizei" and param.is_pointer and not param.is_const and param.len == "1":
            return param
    return None


# ── Vendor-suffix stripping for group names ───────────────────────────────────

# Known vendor/extension suffixes that appear at the end of XML group names.
# Ordered longest-first so e.g. "MESA" is tried before any single-letter suffix.
_VENDOR_SUFFIXES = (
    "MESA",
    "INTEL",
    "APPLE",
    "SGIS",
    "SGIX",
    "SUNX",
    "QCOM",
    "3DFX",
    "INGR",
    "REND",
    "WIN",
    "ARB",
    "EXT",
    "NVX",
    "KHR",
    "OES",
    "AMD",
    "ATI",
    "IBM",
    "SUN",
    "NV",
    "HP",
    "IMG",
    "VIV",
    "DMP",
    "FJ",
)


def _build_group_rename(group_set: set) -> dict[str, str]:
    """Return a mapping old_name → clean_name for every group whose suffix can be stripped
    without colliding with another group already in group_set."""
    rename: dict[str, str] = {}
    for name in group_set:
        for suffix in _VENDOR_SUFFIXES:
            if name.endswith(suffix) and len(name) > len(suffix):
                clean = name[: -len(suffix)]
                # Only rename if the clean name is not already taken by another group
                if clean not in group_set:
                    rename[name] = clean
                break  # try only the first matching suffix
    return rename


# ── Location-type detection (name-based, no XML annotation available) ─────────

_UNIFORM_LOCATION_RETURN: frozenset = frozenset(
    {
        "glGetUniformLocation",
        "glGetFragDataLocation",
        "glGetFragDataIndex",
    }
)

_ATTRIB_LOCATION_RETURN: frozenset = frozenset(
    {
        "glGetAttribLocation",
    }
)


def _is_uniform_location_param(command: Command, param: CommandParam) -> bool:
    """True if this GLint 'location' param is a uniform location."""
    return (
        param.type == "GLint"
        and param.name == "location"
        and (command.name.startswith("glUniform") or command.name.startswith("glProgramUniform"))
    )


def _is_attrib_location_param(command: Command, param: CommandParam) -> bool:
    """True if this GLuint 'index' param is a vertex-attrib location."""
    return (
        param.type == "GLuint"
        and param.name == "index"
        and ("VertexAttrib" in command.name or command.name == "glBindAttribLocation")
    )


class _Capitalizer:
    _EXCLUDED: ClassVar[set[str]] = {"1D", "2D", "3D"}

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

    def convert(self, constant: str, skip_filter: bool = False) -> str:
        parts = constant.replace("GL_", "").split("_")
        parts = [self._capitalizer.capitalize(p) for p in parts]

        if not skip_filter:
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

    def __init__(self, registry: Registry, doc_index: dict[str, FunctionDoc] | None = None):
        super().__init__(registry, doc_index)
        self._capitalizer = _Capitalizer()
        self._handle_classes: dict[str, str] = {}  # class_ string → CamelCase handle name
        self._group_rename: dict[str, str] = {}  # xml group name → clean C++ type name

    @property
    def name(self) -> str:
        return "cpp"

    def generate(self, api: str, version: str) -> dict[str, str]:
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

        handle_types: list[tuple[str, str]] = [
            (n, n) for n in sorted(set(self._handle_classes.values()))
        ]

        # Build enum class context — only include enums in the consolidated set
        self._emitted_groups: set = set()
        enum_classes = []
        for group_name in sorted(group_set):
            all_enums = self.registry.group_to_enums[group_name]
            filtered = [e for e in all_enums if e.name in consolidated.enums]
            if filtered:
                enum_classes.append(self._enum_class_context(group_name, filtered))
                self._emitted_groups.add(group_name)

        self._emitted_bitmask_groups: set = self._emitted_groups & self.registry.bitmask_groups

        # Collect location types used by included commands
        need_uniform_location = False
        need_attrib_location = False
        for command_name in consolidated.commands:
            cmd = self.registry.command_by_name.get(command_name)
            if cmd is None:
                continue
            if cmd.name in _UNIFORM_LOCATION_RETURN or any(
                _is_uniform_location_param(cmd, p) for p in cmd.params
            ):
                need_uniform_location = True
            if cmd.name in _ATTRIB_LOCATION_RETURN or any(
                _is_attrib_location_param(cmd, p) for p in cmd.params
            ):
                need_attrib_location = True

        location_types: list[tuple[str, str]] = []
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
            # For data upload functions (const void* + size), add an ArrayView<T> overload
            upload_params = _find_data_upload_params(command)
            if upload_params:
                data_param, size_param = upload_params
                functions.append(self._array_view_overload_context(command, data_param, size_param))
            # For object-creation functions (glGen*/glCreate*), add a singular convenience functor
            creation_params = _find_object_creation_params(command)
            if creation_params:
                count_param, output_param = creation_params
                if output_param.class_ in self._handle_classes:
                    functions.append(
                        self._single_object_creation_context(command, count_param, output_param)
                    )
            # For object-deletion functions (glDelete*), add a singular convenience functor
            deletion_params = _find_object_deletion_params(command)
            if deletion_params:
                count_param, input_param = deletion_params
                if input_param.class_ in self._handle_classes:
                    functions.append(
                        self._single_object_deletion_context(command, count_param, input_param)
                    )

        cmd_version_map = self._build_command_version_map(api, version)
        functors = self._build_functors(functions, api, cmd_version_map)

        # Build DSA object classes
        dsa_classes = self._build_dsa_classes(consolidated, api)

        # Build enriched-handle wrapper classes (legacy non-DSA companion)
        handle_classes = self._build_handle_classes(consolidated, api)

        # Build RAII smart-pointer resource list
        raii_resources = self._collect_raii_resources(consolidated)

        hpp_name = f"include/glaze/{api}.hpp"
        raii_name = f"include/glaze/{api}_raii.hpp"
        handle_name = f"include/glaze/{api}_handle.hpp"
        context = {
            "api": api,
            "generation_header": self._generation_header(api, version, "C++"),
            "handle_types": handle_types,
            "location_types": location_types,
            "enum_classes": enum_classes,
            "functors": functors,
            "dsa_classes": dsa_classes,
            "resources": raii_resources,
        }
        raii_context = {
            "api": api,
            "generation_header": self._generation_header(api, version, "C++ RAII"),
            "resources": raii_resources,
        }
        handle_context = {
            "api": api,
            "generation_header": self._generation_header(api, version, "C++ Handle"),
            "handle_classes": handle_classes,
        }
        return {
            hpp_name: self._render_template("cpp/gl.hpp.j2", context),
            raii_name: self._render_template("cpp/gl_raii.hpp.j2", raii_context),
            handle_name: self._render_template("cpp/gl_handle.hpp.j2", handle_context),
        }

    # ----------------------------------------------------------------- RAII

    def _collect_raii_resources(self, consolidated: ConsolidatedRequire) -> list[dict]:
        """Collect RAII smart-pointer resource entries from consolidated commands.

        Detects two patterns and groups them by GL object class:

        - Multi-object: glGenBuffers/glDeleteBuffers, glGenTextures/glDeleteTextures,
          glCreateBuffers/glDeleteBuffers (DSA), etc. — both creator and deleter take
          (GLsizei n, GLuint*) and have a class_ attribute on the array param.
        - Singular: glCreateProgram/glDeleteProgram, glCreateShader/glDeleteShader —
          creator returns GLuint, deleter takes a single GLuint with class_.

        Returns a list of dicts (sorted by alias) suitable for the gl_raii.hpp.j2
        template; each entry has keys: alias, handle_type, gen_func_name,
        delete_func_name.
        """
        creators: dict[str, tuple[Command, str]] = {}
        deleters: dict[str, tuple[Command, str]] = {}

        for command_name in consolidated.commands:
            cmd = self.registry.command_by_name.get(command_name)
            if cmd is None:
                continue

            # Multi-object creator (glGen*/glCreate* + count + GLuint*)
            create_pair = _find_object_creation_params(cmd)
            if create_pair is not None:
                _, output_param = create_pair
                cls = output_param.class_
                if cls and cls in self._handle_classes:
                    plural = self._convert_function_name(cmd.name)
                    singular = plural.rstrip("s") if plural.endswith("s") else plural
                    creators[cls] = (cmd, singular)
                    continue

            # Multi-object deleter (glDelete* + count + const GLuint*)
            delete_pair = _find_object_deletion_params(cmd)
            if delete_pair is not None:
                _, input_param = delete_pair
                cls = input_param.class_
                if cls and cls in self._handle_classes:
                    plural = self._convert_function_name(cmd.name)
                    singular = plural.rstrip("s") if plural.endswith("s") else plural
                    deleters[cls] = (cmd, singular)
                    continue

            # Singular creator: glCreate<Name>() -> GLuint, no GLuint* output param
            handle_alias = self._detect_handle_return(cmd)
            if handle_alias is not None:
                # Find which class_ this handle alias maps to
                for cls_str, alias in self._handle_classes.items():
                    if alias == handle_alias:
                        creators[cls_str] = (cmd, self._convert_function_name(cmd.name))
                        break
                continue

            # Singular deleter: glDelete<Name>(GLuint name)
            if cmd.name.startswith("glDelete") and len(cmd.params) == 1:
                p = cmd.params[0]
                if (
                    p.type == "GLuint"
                    and not p.is_pointer
                    and p.class_
                    and p.class_ in self._handle_classes
                ):
                    deleters[p.class_] = (cmd, self._convert_function_name(cmd.name))

        resources: list[dict] = []
        for cls in sorted(set(creators) & set(deleters)):
            create_cmd, gen_func_name = creators[cls]
            delete_cmd, delete_func_name = deleters[cls]
            # Use the unsuffixed CamelCase name for ergonomic Unique<X>/Shared<X>
            # aliases, even when the handle type itself was Id-suffixed to avoid
            # collisions with enum class names.
            alias = _to_handle_name(cls)
            resources.append({
                "alias": alias,
                "handle_type": self._handle_classes[cls],
                "gen_gl_name": create_cmd.name,
                "gen_func_name": gen_func_name,
                "creator_struct": _functor_struct_name(gen_func_name),
                "delete_gl_name": delete_cmd.name,
                "delete_func_name": delete_func_name,
                "deleter_struct": _functor_struct_name(delete_func_name),
            })
        return resources

    # --------------------------------------------------------------- functors

    def _build_functors(self, functions: list, api: str, cmd_version_map: dict[str, str]) -> list:
        """Group function contexts by func_name; each group becomes one functor struct."""
        seen: dict[str, dict] = {}
        order: list[str] = []
        for fn in functions:
            fname = fn["func_name"]
            if fname not in seen:
                struct_name = fname[0].upper() + fname[1:] + "Fn"
                doc = self.doc_index.get(fn["gl_name"])
                gl_name = fn["gl_name"]
                ver = cmd_version_map.get(gl_name)
                ver_tag = f" [{api.upper()} {ver}]" if ver else ""
                doc_brief = f"{doc.brief}{ver_tag}" if doc else (ver_tag.strip() if ver_tag else None)
                seen[fname] = {
                    "struct_name": struct_name,
                    "func_name": fname,
                    "gl_name": gl_name,
                    "overloads": [],
                    "doc_brief": doc_brief,
                    "doc_params": doc.params if doc else {},
                }
                order.append(fname)
            seen[fname]["overloads"].append(
                {
                    "return_type": fn["return_type"],
                    "params_str": fn["params_str"],
                    "call_args_str": fn.get("call_args_str", ""),
                    "body": fn.get("body"),
                    "template_prefix": fn.get("template_prefix"),
                }
            )
        return [seen[n] for n in order]

    # ------------------------------------------------------------ DSA classes

    def _build_dsa_classes(self, consolidated: object, api: str) -> list:
        """Build context dicts for DSA wrapper classes from object_dict."""
        dsa_classes = []
        for class_str in sorted(self.registry.object_dict):
            if class_str not in self._handle_classes:
                continue
            handle_name = self._handle_classes[class_str]
            class_name = _to_handle_name(class_str)
            commands = self.registry.object_dict[class_str]
            # Only include commands in the consolidated set
            filtered = [cmd for cmd in commands if cmd.name in consolidated.commands]
            if not filtered:
                continue

            methods = []
            for cmd in sorted(filtered, key=lambda c: c.name):
                method_name = self._dsa_method_name(cmd.name, class_str)
                if method_name in _CPP_KEYWORDS:
                    method_name = method_name + "_"
                # Skip the first param (the handle)
                method_params = cmd.params[1:]
                params_str = ", ".join(
                    self._generate_dsa_param_decl(p, cmd, api) for p in method_params
                )
                call_args = ["m_id.id"] + [
                    self._generate_call_arg(p, cmd) for p in method_params
                ]
                call_args_str = ", ".join(call_args)
                return_type_str = cmd.return_type_str or cmd.return_type.to_c_string()
                methods.append({
                    "name": method_name,
                    "return_type": return_type_str,
                    "params_str": params_str,
                    "gl_name": cmd.name,
                    "call_args_str": call_args_str,
                })

            dsa_classes.append({
                "class_name": class_name,
                "handle_type": handle_name,
                "methods": methods,
            })
        return dsa_classes

    def _build_handle_classes(self, consolidated: ConsolidatedRequire, api: str) -> list:
        """Build context dicts for the enriched-handle wrapper classes.

        Same shape as `_build_dsa_classes` but excludes DSA (Named*) commands and
        delegates each method to the corresponding free-function functor in `gl::`,
        so the wrapper inherits the functor's strong return type (e.g.
        UniformLocation), type-safe param substitution and GLAZE_GL_CHECK.
        """
        handle_classes = []
        for class_str in sorted(self.registry.object_dict):
            if class_str not in self._handle_classes:
                continue
            handle_name = self._handle_classes[class_str]
            class_name = _to_handle_name(class_str)
            commands = self.registry.object_dict[class_str]
            filtered = [
                cmd for cmd in commands
                if cmd.name in consolidated.commands and "Named" not in cmd.name
            ]
            if not filtered:
                continue

            methods = []
            for cmd in sorted(filtered, key=lambda c: c.name):
                method_name = self._dsa_method_name(cmd.name, class_str)
                if method_name in _CPP_KEYWORDS:
                    method_name = method_name + "_"

                # Reuse _function_context to derive the *wrapped* return type and
                # the canonical functor name. This guarantees that
                # glGetUniformLocation → "UniformLocation", glGetString → "std::string",
                # etc., matching the binding's free functions exactly.
                fn_ctx = self._function_context(cmd)
                functor_name = fn_ctx["func_name"]
                return_type = fn_ctx["return_type"]

                method_params = cmd.params[1:]
                params_str = ", ".join(
                    self._generate_dsa_param_decl(p, cmd, api) for p in method_params
                )
                # Pass-through call args: m_id is already a strong handle, and
                # the other params are strong-typed in the method signature, so
                # forward them by name. The functor's operator() does any
                # `.id`/`.loc`/cast extraction internally.
                forwarded = ", ".join(["m_id"] + [p.name for p in method_params])

                methods.append({
                    "name": method_name,
                    "return_type": return_type,
                    "params_str": params_str,
                    "functor_name": functor_name,
                    "gl_name": cmd.name,
                    "call_args_str": forwarded,
                })

            handle_classes.append({
                "class_name": class_name,
                "handle_type": handle_name,
                "methods": methods,
            })
        return handle_classes

    def _dsa_method_name(self, gl_name: str, class_str: str) -> str:
        """Convert a GL command name to a DSA method name.

        Examples:
            glNamedBufferData + "buffer" → "data"
            glNamedBufferSubData + "buffer" → "subData"
            glClearNamedBufferData + "buffer" → "clearData"
        """
        # Strip 'gl' prefix
        name = gl_name[2:]
        # Remove 'Named' (DSA marker)
        name = name.replace("Named", "")
        # Remove class name (capitalized words)
        class_camel = _to_handle_name(class_str)
        if name.startswith(class_camel):
            name = name[len(class_camel):]
        elif class_camel in name:
            name = name.replace(class_camel, "", 1)
        # Lowercase first letter
        name = name[0].lower() + name[1:] if name else self._convert_function_name(gl_name)
        return name

    # ----------------------------------------------------------------- checks

    # --------------------------------------------------------------- contexts

    def _enum_class_context(self, group_name: str, enums: list[Enum]) -> dict:
        clean_name = self._group_rename.get(group_name, group_name)
        base_type = "GLboolean" if clean_name == "Boolean" else "GLenum"
        converter = _EnumIdentifierConverter(clean_name, self._capitalizer)
        seen_names: set[str] = set()
        entries = []
        for e in enums:
            name = converter.convert(e.name)
            if name in seen_names:
                # Collision: retry without filtering group-name parts
                name = converter.convert(e.name, skip_filter=True)
            if name not in seen_names:
                seen_names.add(name)
                entries.append({"name": name, "value": e.name})
        is_bitmask = group_name in self.registry.bitmask_groups
        return {"group_name": clean_name, "base_type": base_type, "entries": entries, "is_bitmask": is_bitmask}

    def _function_context(self, command: Command) -> dict:
        return_type_str = command.return_type_str or command.return_type.to_c_string()
        func_name = self._convert_function_name(command.name)
        params_str = ", ".join(self._generate_param_decl(p, command) for p in command.params)
        call_args_str = ", ".join(self._generate_call_arg(p, command) for p in command.params)

        # Pattern A: const GLubyte* / const GLchar* returns → wrap to std::string
        if _is_string_return_command(command):
            raw_type = command.return_type.name
            body = (
                f"const {raw_type}* raw = {command.name}({call_args_str});\n"
                "return raw ? reinterpret_cast<const char*>(raw) : std::string{};"
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
            body = f"return UniformLocation({command.name}({call_args_str}));"
            return {
                "return_type": "UniformLocation",
                "func_name": func_name,
                "params_str": params_str,
                "gl_name": command.name,
                "body": body,
            }

        if command.name in _ATTRIB_LOCATION_RETURN:
            body = f"return AttribLocation({command.name}({call_args_str}));"
            return {
                "return_type": "AttribLocation",
                "func_name": func_name,
                "params_str": params_str,
                "gl_name": command.name,
                "body": body,
            }

        # Handle-returning create commands: glCreateProgram() -> Program
        handle_type = self._detect_handle_return(command)
        if handle_type:
            body = f"return {handle_type}({command.name}({call_args_str}));"
            return {
                "return_type": handle_type,
                "func_name": func_name,
                "params_str": params_str,
                "gl_name": command.name,
                "body": body,
            }

        return {
            "return_type": return_type_str,
            "func_name": func_name,
            "params_str": params_str,
            "call_args_str": call_args_str,
            "gl_name": command.name,
            "body": None,
        }

    def _string_overload_context(
        self, command: Command, string_param: CommandParam, length_param: CommandParam | None
    ) -> dict:
        """Build a std::string-returning overload for Pattern B string-output commands."""
        func_name = self._convert_function_name(command.name)

        # Overload params: all original params except the GLchar* buffer and GLsizei* length
        overload_params = [
            p for p in command.params if p is not string_param and p is not length_param
        ]
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
            f"std::string result(static_cast<std::size_t>({string_param.len}), '\\0');\n"
            "GLsizei length = 0;\n"
            f"{command.name}({call_args_str});\n"
            "result.resize(static_cast<std::size_t>(length));\n"
            "return result;"
        )
        return {
            "return_type": "std::string",
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
        }

    def _array_view_overload_context(
        self, command: Command, data_param: CommandParam, size_param: CommandParam
    ) -> dict:
        """Build an ArrayView<T>-accepting overload for data upload commands."""
        func_name = self._convert_function_name(command.name)

        # Overload params: replace data and size params with a single ArrayView<T>
        overload_params = [p for p in command.params if p is not data_param and p is not size_param]
        param_decls = [self._generate_param_decl(p, command) for p in overload_params]
        param_decls.append("const ArrayView<T>& data")
        params_str = ", ".join(param_decls)

        # Build call args: substitute size with data.size_bytes() and data ptr with data.data()
        call_parts = []
        for p in command.params:
            if p is size_param:
                call_parts.append(f"static_cast<{size_param.type}>(data.size_bytes())")
            elif p is data_param:
                call_parts.append("data.data()")
            else:
                call_parts.append(self._generate_call_arg(p, command))
        call_args_str = ", ".join(call_parts)

        return_type_str = command.return_type_str or command.return_type.to_c_string()
        body = f"return ::{command.name}({call_args_str});"
        return {
            "return_type": return_type_str,
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
            "template_prefix": "template<typename T>",
        }

    def _single_object_creation_context(
        self, command: Command, count_param: CommandParam, output_param: CommandParam
    ) -> dict:
        """Build a singular convenience functor for object-creation commands.

        Turns glGenBuffers(n, buffers) into genBuffer() -> BufferId.
        """
        handle_name = self._handle_classes[output_param.class_]
        # Singular func name: strip trailing 's' (genBuffers → genBuffer)
        plural_name = self._convert_function_name(command.name)
        func_name = plural_name.rstrip("s") if plural_name.endswith("s") else plural_name

        # Extra params: anything that isn't the count or the output pointer
        extra_params = [
            p for p in command.params if p is not count_param and p is not output_param
        ]
        params_str = ", ".join(self._generate_param_decl(p, command) for p in extra_params)

        # Build call args: 1 for count, &obj for output, pass-through for extras
        call_parts = []
        for p in command.params:
            if p is count_param:
                call_parts.append("1")
            elif p is output_param:
                call_parts.append("reinterpret_cast<GLuint*>(&obj)")
            else:
                call_parts.append(self._generate_call_arg(p, command))
        call_args_str = ", ".join(call_parts)

        body = (
            f"{handle_name} obj;\n"
            f"::{command.name}({call_args_str});\n"
            "return obj;"
        )
        return {
            "return_type": handle_name,
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
        }

    def _single_object_deletion_context(
        self, command: Command, count_param: CommandParam, input_param: CommandParam
    ) -> dict:
        """Build a singular convenience functor for object-deletion commands.

        Turns glDeleteBuffers(n, buffers) into deleteBuffer(BufferId obj) -> void.
        """
        handle_name = self._handle_classes[input_param.class_]
        plural_name = self._convert_function_name(command.name)
        func_name = plural_name.rstrip("s") if plural_name.endswith("s") else plural_name

        # Extra params: anything that isn't the count or the input pointer
        extra_params = [
            p for p in command.params if p is not count_param and p is not input_param
        ]
        param_decls = [self._generate_param_decl(p, command) for p in extra_params]
        param_decls.append(f"{handle_name} obj")
        params_str = ", ".join(param_decls)

        # Build call args
        call_parts = []
        for p in command.params:
            if p is count_param:
                call_parts.append("1")
            elif p is input_param:
                call_parts.append("reinterpret_cast<const GLuint*>(&obj)")
            else:
                call_parts.append(self._generate_call_arg(p, command))
        call_args_str = ", ".join(call_parts)

        body = f"::{command.name}({call_args_str});"
        return {
            "return_type": "void",
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
        }

    def _detect_handle_return(self, command: Command) -> str | None:
        """Detect if a command returns a GLuint that should be wrapped as a handle type.

        Matches commands like glCreateProgram() -> GLuint where 'Program' maps to a known handle.
        """
        rt = command.return_type
        if rt.name != "GLuint" or rt.is_pointer:
            return None
        if not command.name.startswith("glCreate"):
            return None
        # Extract the object name from glCreate<Name>
        suffix = command.name[len("glCreate"):]
        # Match against known handle classes (case-insensitive)
        for class_str, handle_name in self._handle_classes.items():
            class_camel = _to_handle_name(class_str)
            if suffix == class_camel:
                return handle_name
        return None

    # ----------------------------------------------------------- param helpers

    def _convert_function_name(self, gl_name: str) -> str:
        # glClearColor → clearColor
        return gl_name[2:3].lower() + gl_name[3:]

    def _generate_param_decl(self, param: CommandParam, command: Command | None = None) -> str:
        return f"{self._param_type_str(param, command=command)} {param.name}"

    def _generate_dsa_param_decl(
        self, param: CommandParam, command: Command, api: str
    ) -> str:
        """Like _generate_param_decl but fully qualifies types that could collide in the dsa namespace."""
        needs_qualify = (
            (param.class_ and _is_uint_handle(param) and param.class_ in self._handle_classes)
            or (param.has_group() and param.group in self._emitted_groups)
        )
        if needs_qualify:
            type_str = self._param_type_str(param, command=command, namespace_prefix=f"::{api}::")
        else:
            type_str = self._param_type_str(param, command=command)
        return f"{type_str} {param.name}"

    def _param_type_str(
        self,
        param: CommandParam,
        ignore_group: bool = False,
        command: Command | None = None,
        namespace_prefix: str = "",
    ) -> str:
        """Reconstruct the parameter type string, substituting enum class or handle name if applicable.

        When namespace_prefix is set (e.g. '::gl::'), it is prepended to the
        substituted type name only — not to const/pointer qualifiers.
        """
        # Location type substitution (name-based heuristic)
        if command:
            if _is_uniform_location_param(command, param):
                return f"{namespace_prefix}UniformLocation"
            if _is_attrib_location_param(command, param):
                return f"{namespace_prefix}AttribLocation"

        # Strong handle substitution: GLuint → Texture / Buffer / etc.
        if param.class_ and _is_uint_handle(param) and param.class_ in self._handle_classes:
            handle_name = self._handle_classes[param.class_]
            parts = []
            for part in param.type_parts:
                if part == "GLuint":
                    parts.append(f"{namespace_prefix}{handle_name}")
                elif part:
                    parts.append(part)
            return " ".join(parts)

        use_type = param.type
        if not ignore_group and param.has_group() and param.group in self._emitted_groups:
            clean = self._group_rename.get(param.group, param.group)
            if param.group in self._emitted_bitmask_groups and not param.is_pointer:
                use_type = f"Flags<{clean}>" if not namespace_prefix else f"{namespace_prefix}Flags<{namespace_prefix}{clean}>"
            else:
                use_type = f"{namespace_prefix}{clean}" if namespace_prefix else clean

        parts = []
        for part in param.type_parts:
            if part == param.type:
                parts.append(use_type if use_type else part)
            elif part:
                parts.append(part)
        return " ".join(parts)

    def _generate_call_arg(self, param: CommandParam, command: Command | None = None) -> str:
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
            if param.group in self._emitted_bitmask_groups:
                return f"static_cast<{raw_type}>({param.name}.value())"
            return f"static_cast<{raw_type}>({param.name})"

        if self._type_must_change(param) and param.is_pointer:
            const_str = "const " if param.is_const else ""
            return f"reinterpret_cast<{const_str}{param.type}*>({param.name})"

        return param.name

    def _type_must_change(self, param: CommandParam) -> bool:
        return param.has_group() and param.group in self.registry.group_to_enums
