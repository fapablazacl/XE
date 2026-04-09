from typing import ClassVar

from glaze.doc_parser import FunctionDoc
from glaze.generators.base import Generator
from glaze.model import Command, CommandParam, ConsolidatedRequire, Enum, Registry
from glaze.utils.string_utils import split_capitalized, version_to_int

_CPP_KEYWORDS: frozenset = frozenset(
    {
        "delete",
        "new",
        "class",
        "template",
        "operator",
        "return",
        "switch",
        "case",
        "default",
        "break",
        "continue",
        "if",
        "else",
        "for",
        "while",
        "do",
        "void",
        "int",
        "float",
        "double",
        "bool",
        "char",
        "namespace",
        "using",
        "static",
        "const",
        "virtual",
        "public",
        "private",
        "protected",
        "struct",
        "enum",
        "union",
        "typedef",
        "extern",
        "inline",
        "volatile",
        "register",
        "auto",
        "throw",
        "try",
        "catch",
        "this",
        "sizeof",
        "true",
        "false",
    }
)


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
        if param.type == "GLuint" and param.is_pointer and param.is_const and param.class_:
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
        if param.type == "GLuint" and param.is_pointer and not param.is_const and param.class_:
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


# Scalar pointer types accepted as the trailing output parameter of a per-object
# "getter" query (glGetShaderiv, glGetProgramiv, glGetBufferParameteriv, …).
_SCALAR_QUERY_TYPES: frozenset = frozenset(
    {
        "GLint",
        "GLuint",
        "GLfloat",
        "GLdouble",
        "GLboolean",
        "GLint64",
        "GLuint64",
    }
)


def _find_scalar_query_param(command: Command) -> CommandParam | None:
    """Return the trailing scalar output pointer param of a per-object query, or None.

    Matches commands of the form ``glGet<Obj><...>v(<handle>, ..., T *out)``,
    where the first parameter is a named GL object handle and the last is the
    only non-const output pointer. Used to synthesize a convenience overload
    that returns the scalar directly instead of requiring the caller to pass a
    pointer to a local (e.g. ``getShaderiv(shader, eCompileStatus) -> GLint``).

    Restricted to handle-first commands so that fetching ``glGetIntegerv`` /
    ``glGetFloatv`` — which may legitimately write multiple values — does not
    accidentally get a single-slot overload that would corrupt the stack.
    """
    if not command.name.startswith("glGet"):
        return None
    if len(command.params) < 2:
        return None
    first = command.params[0]
    if not (first.class_ and first.type == "GLuint" and not first.is_pointer):
        return None
    last = command.params[-1]
    if (
        last.type not in _SCALAR_QUERY_TYPES
        or not last.is_pointer
        or last.is_const
    ):
        return None
    # Ensure `last` is the only non-const output pointer.
    for p in command.params[1:-1]:
        if p.is_pointer and not p.is_const:
            return None
    return last


# Commands whose info log length can be queried via an adjacent ``Get*iv``
# functor. Used to synthesize a zero-extra-argument std::string overload that
# internally sizes the buffer via GL_INFO_LOG_LENGTH, removing the need for
# callers to chain two calls manually. Keyed by the C command name (the body
# still emits the raw ``::glGet*`` calls so it is independent of handle-type
# naming collisions).
_INFOLOG_SELF_QUERY_MAP: dict[str, str] = {
    "glGetShaderInfoLog": "::glGetShaderiv",
    "glGetProgramInfoLog": "::glGetProgramiv",
}


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

    def __init__(
        self,
        registry: Registry,
        doc_index: dict[str, FunctionDoc] | None = None,
        extension_vendors: list[str] | None = None,
        extension_names: list[str] | None = None,
    ):
        super().__init__(registry, doc_index, extension_vendors, extension_names)
        self._capitalizer = _Capitalizer()
        self._handle_classes: dict[str, str] = {}  # class_ string → CamelCase handle name
        self._group_rename: dict[str, str] = {}  # xml group name → clean C++ type name

    @property
    def name(self) -> str:
        return "cpp"

    def generate(self, api: str, version: str) -> dict[str, str]:
        self._check_api_version(api, version)
        consolidated = self.registry.consolidate(api, version)

        # Extension emissions: per-extension Commands/Enums NOT already in core.
        ext_emissions = self._collect_extension_emissions(api, version)
        extension_command_names: set[str] = set()
        extension_enum_names: set[str] = set()
        cmd_to_extension_short: dict[str, str] = {}
        for ext in ext_emissions:
            for cmd in ext["commands"]:
                extension_command_names.add(cmd.name)
                cmd_to_extension_short[cmd.name] = ext["short_name"]
            for enum in ext["enums"]:
                extension_enum_names.add(enum.name)
        # Discovery (handle classes, enum groups) walks core + extensions so
        # extension-only commands contribute their handle types and enum
        # groups to the bindings. The dsa/handle/raii pipelines still filter
        # by `consolidated.commands` and stay core-only.
        discovery_command_names = set(consolidated.commands) | extension_command_names
        union_enum_names: set[str] = set(consolidated.enums) | extension_enum_names

        # Single pass: collect handle classes and enum groups from all included commands
        self._handle_classes = {}
        group_set: set = set()
        for command_name in discovery_command_names:
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

        # Build enum class context — include enum entries from the union of
        # core + extension emissions, so an extension-only entry naturally
        # appears inside its (core or extension) enum class.
        self._emitted_groups: set = set()
        enum_classes = []
        for group_name in sorted(group_set):
            all_enums = self.registry.group_to_enums[group_name]
            filtered = [e for e in all_enums if e.name in union_enum_names]
            if filtered:
                enum_classes.append(self._enum_class_context(group_name, filtered))
                self._emitted_groups.add(group_name)

        self._emitted_bitmask_groups: set = self._emitted_groups & self.registry.bitmask_groups

        # Collect location types used by included commands (core + extensions)
        need_uniform_location = False
        need_attrib_location = False
        for command_name in discovery_command_names:
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
        functions: list = []
        for command_name in sorted(consolidated.commands):
            command = self.registry.command_by_name.get(command_name)
            if command is None:
                continue
            self._append_command_function_contexts(functions, command)

        # Append extension function contexts, tagged with `extension_short_name`
        # so the template (and `_build_functors`) can wrap them in their per-
        # extension `#ifndef GLAZE_GL_NO_EXT_<short>` guard.
        for ext in ext_emissions:
            short = ext["short_name"]
            for cmd in sorted(ext["commands"], key=lambda c: c.name):
                start = len(functions)
                self._append_command_function_contexts(functions, cmd)
                for fn_ctx in functions[start:]:
                    fn_ctx["extension_short_name"] = short

        cmd_version_map = self._build_command_version_map(api, version)
        functors = self._build_functors(functions, api, cmd_version_map)

        # Build DSA object classes
        dsa_classes = self._build_dsa_classes(consolidated, api, cmd_version_map)

        # Build enriched-handle wrapper classes (legacy non-DSA companion)
        handle_classes = self._build_handle_classes(consolidated, api, cmd_version_map)

        # Build RAII smart-pointer resource list
        raii_resources = self._collect_raii_resources(consolidated, api)
        # Annotate each resource with the GL version at which both creator and
        # deleter become available — used to gate HandleTraits and Traits
        # specializations behind #if GLAZE_GL_VERSION >= NN.
        for r in raii_resources:
            gen_ver = cmd_version_map.get(r["gen_gl_name"])
            del_ver = cmd_version_map.get(r["delete_gl_name"])
            gen_int = version_to_int(gen_ver) if gen_ver else 0
            del_int = version_to_int(del_ver) if del_ver else 0
            r["version_int"] = max(gen_int, del_int)

        # Bridge per-resource creator/deleter pairs to the wrapper classes that
        # exist in each family (DSA and legacy "handle::"). The result feeds the
        # glaze::Traits<T> specialization blocks emitted at the bottom of
        # gl.hpp.j2 (DSA + raw handles) and gl_handle.hpp.j2 (legacy wrappers).
        dsa_proxy_map = {c["handle_type"]: c["class_name"] for c in dsa_classes}
        handle_proxy_map = {c["handle_type"]: c["class_name"] for c in handle_classes}
        dsa_handles_with_proxy = [
            {
                "handle_type": r["handle_type"],
                "class_name": dsa_proxy_map[r["handle_type"]],
                "gen_func_name": r["gen_func_name"],
                "delete_func_name": r["delete_func_name"],
                "version_int": r["version_int"],
                "create_params_str": r["create_params_str"],
                "create_call_args_str": r["create_call_args_str"],
            }
            for r in raii_resources
            if r["handle_type"] in dsa_proxy_map
        ]
        handle_handles_with_proxy = [
            {
                "handle_type": r["handle_type"],
                "class_name": handle_proxy_map[r["handle_type"]],
                "gen_func_name": r["gen_func_name"],
                "delete_func_name": r["delete_func_name"],
                "version_int": r["version_int"],
                "create_params_str": r["create_params_str"],
                "create_call_args_str": r["create_call_args_str"],
            }
            for r in raii_resources
            if r["handle_type"] in handle_proxy_map
        ]

        # Note: GLAZE_GL_VERSION and the GL_VERSION_X_Y feature macros are
        # emitted by the C header (`{api}.h`), which `gl.hpp` already includes.
        # The C++ template just `#if`-gates against the macros that are already
        # in scope, so no extra context is needed for the version gating itself.

        # Extension tokens for the gl::exts namespace and the gl::supports
        # overloads. Each entry mirrors the C-side flag variable so the
        # constexpr `Extension` initializer can take its address.
        extensions_context = [
            {
                "name": ext["name"],
                "short_name": ext["short_name"],
                "guard_macro": ext["guard_macro"],
                "flag_var": ext["flag_var"],
            }
            for ext in ext_emissions
        ]

        hpp_name = f"include/glaze/{api}.hpp"
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
            "dsa_handles_with_proxy": dsa_handles_with_proxy,
            "extensions": extensions_context,
        }
        handle_context = {
            "api": api,
            "generation_header": self._generation_header(api, version, "C++ Handle"),
            "handle_classes": handle_classes,
            "handle_handles_with_proxy": handle_handles_with_proxy,
        }
        return {
            hpp_name: self._render_template("cpp/gl.hpp.j2", context),
            handle_name: self._render_template("cpp/gl_handle.hpp.j2", handle_context),
            # Static, API-agnostic smart-pointer header. Shipped alongside the
            # generated per-API headers so a standalone CLI invocation produces
            # a self-contained include tree.
            "include/glaze/raii.hpp": self._read_static_template("cpp/static/raii.hpp"),
        }

    # ----------------------------------------------------------------- RAII

    def _collect_raii_resources(self, consolidated: ConsolidatedRequire, api: str) -> list[dict]:
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

            # Determine the leading (non count/output) params of the selected
            # creator and propagate them into Traits::create(...):
            #
            #   • Singular creator (glCreateShader, glCreateProgram, …) →
            #     all params are "extra". glCreateShader contributes
            #     ShaderType type; glCreateProgram contributes nothing.
            #   • Multi-object (glGen*/glCreate* + count + GLuint*) → params
            #     that are neither the count nor the output pointer.
            #     glGenBuffers / glCreateBuffers contribute nothing;
            #     glCreateQueries(target, n, ids) and glCreateTextures(
            #     target, n, textures) contribute `target`.
            #
            # These are exactly the params _single_object_creation_context
            # keeps on the singular wrapper's operator(), so the Traits
            # signature and the functor signature stay in lock-step.
            #
            # The body calls the C++ functor (e.g. ::gl::createShader), which
            # already accepts the strong types used in the signature, so
            # arguments are forwarded verbatim — no static_cast is needed
            # (and adding one would fail to compile, since the functor does
            # not accept the raw GLenum/GLuint form).
            creation_pair = _find_object_creation_params(create_cmd)
            if creation_pair is not None:
                count_param, output_param = creation_pair
                extra_params = [
                    p for p in create_cmd.params if p is not count_param and p is not output_param
                ]
            else:
                extra_params = list(create_cmd.params)
            create_params_str = ", ".join(
                f"{self._param_type_str(p, command=create_cmd, namespace_prefix=f'::{api}::')} {p.name}"
                for p in extra_params
            )
            create_call_args_str = ", ".join(p.name for p in extra_params)

            resources.append(
                {
                    "alias": alias,
                    "handle_type": self._handle_classes[cls],
                    "gen_gl_name": create_cmd.name,
                    "gen_func_name": gen_func_name,
                    "creator_struct": _functor_struct_name(gen_func_name),
                    "delete_gl_name": delete_cmd.name,
                    "delete_func_name": delete_func_name,
                    "deleter_struct": _functor_struct_name(delete_func_name),
                    "create_params_str": create_params_str,
                    "create_call_args_str": create_call_args_str,
                }
            )
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
                doc_brief = (
                    f"{doc.brief}{ver_tag}" if doc else (ver_tag.strip() if ver_tag else None)
                )
                seen[fname] = {
                    "struct_name": struct_name,
                    "func_name": fname,
                    "gl_name": gl_name,
                    "overloads": [],
                    "doc_brief": doc_brief,
                    "doc_params": doc.params if doc else {},
                    "version_int": version_to_int(ver) if ver else 0,
                    "extension_short_name": fn.get("extension_short_name"),
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

    def _build_dsa_classes(
        self, consolidated: object, api: str, cmd_version_map: dict[str, str]
    ) -> list:
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
                call_args = ["m_id.id"] + [self._generate_call_arg(p, cmd) for p in method_params]
                call_args_str = ", ".join(call_args)
                return_type_str = cmd.return_type_str or cmd.return_type.to_c_string()
                ver = cmd_version_map.get(cmd.name)
                methods.append(
                    {
                        "name": method_name,
                        "return_type": return_type_str,
                        "params_str": params_str,
                        "gl_name": cmd.name,
                        "call_args_str": call_args_str,
                        "version_int": version_to_int(ver) if ver else 0,
                    }
                )

            dsa_classes.append(
                {
                    "class_name": class_name,
                    "handle_type": handle_name,
                    "methods": methods,
                }
            )
        return dsa_classes

    def _build_handle_classes(
        self, consolidated: ConsolidatedRequire, api: str, cmd_version_map: dict[str, str]
    ) -> list:
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
                cmd
                for cmd in commands
                if cmd.name in consolidated.commands and "Named" not in cmd.name
            ]
            if not filtered:
                continue

            methods = []
            for cmd in sorted(filtered, key=lambda c: c.name):
                methods.extend(
                    self._build_handle_methods_for_command(
                        cmd, class_str, api, cmd_version_map
                    )
                )

            handle_classes.append(
                {
                    "class_name": class_name,
                    "handle_type": handle_name,
                    "methods": methods,
                }
            )
        return handle_classes

    def _build_handle_methods_for_command(
        self,
        cmd: Command,
        class_str: str,
        api: str,
        cmd_version_map: dict[str, str],
    ) -> list[dict]:
        """Produce one or more handle-class method entries for ``cmd``.

        Emits the canonical method plus any convenience overloads the free
        functor exposes (scalar-return query, std::string-return, zero-arg
        InfoLog self-query). Each method substitutes handle-valued parameters
        with their corresponding ``::{api}::handle::*`` wrapper so wrapper
        classes reference each other rather than the raw handle-id types.
        """
        base_name = self._dsa_method_name(cmd.name, class_str)
        if base_name in _CPP_KEYWORDS:
            base_name = base_name + "_"

        fn_ctx = self._function_context(cmd)
        functor_name = fn_ctx["func_name"]
        canonical_return = fn_ctx["return_type"]

        ver = cmd_version_map.get(cmd.name)
        ver_int = version_to_int(ver) if ver else 0

        methods: list[dict] = []

        # Canonical overload — keep every param except the leading handle.
        canonical_params = list(cmd.params[1:])
        methods.append(
            self._emit_handle_method(
                cmd=cmd,
                method_name=base_name,
                return_type=canonical_return,
                functor_name=functor_name,
                api=api,
                version_int=ver_int,
                kept_params=canonical_params,
                extra_forward=None,
            )
        )

        # Scalar query overload (drops the trailing scalar output pointer).
        scalar_param = _find_scalar_query_param(cmd)
        if scalar_param is not None:
            kept = [p for p in canonical_params if p is not scalar_param]
            methods.append(
                self._emit_handle_method(
                    cmd=cmd,
                    method_name=base_name,
                    return_type=scalar_param.type,
                    functor_name=functor_name,
                    api=api,
                    version_int=ver_int,
                    kept_params=kept,
                    extra_forward=None,
                )
            )

        # std::string output overload (drops the GLchar* buffer + length).
        string_param = _find_string_output_param(cmd)
        if string_param is not None:
            length_param = _find_length_param(cmd)
            kept = [
                p
                for p in canonical_params
                if p is not string_param and p is not length_param
            ]
            methods.append(
                self._emit_handle_method(
                    cmd=cmd,
                    method_name=base_name,
                    return_type="std::string",
                    functor_name=functor_name,
                    api=api,
                    version_int=ver_int,
                    kept_params=kept,
                    extra_forward=None,
                )
            )

        # InfoLog self-query overload (no extra args; sizes itself via Get*iv).
        if cmd.name in _INFOLOG_SELF_QUERY_MAP:
            methods.append(
                self._emit_handle_method(
                    cmd=cmd,
                    method_name=base_name,
                    return_type="std::string",
                    functor_name=functor_name,
                    api=api,
                    version_int=ver_int,
                    kept_params=[],
                    extra_forward=None,
                )
            )

        return methods

    def _emit_handle_method(
        self,
        cmd: Command,
        method_name: str,
        return_type: str,
        functor_name: str,
        api: str,
        version_int: int,
        kept_params: list[CommandParam],
        extra_forward: list[str] | None,
    ) -> dict:
        """Assemble a single handle-class method entry for the given overload."""
        params_str = ", ".join(
            self._generate_handle_param_decl(p, cmd, api) for p in kept_params
        )
        forwarded = ["m_id"] + [self._handle_method_call_arg(p) for p in kept_params]
        if extra_forward:
            forwarded.extend(extra_forward)
        return {
            "name": method_name,
            "return_type": return_type,
            "params_str": params_str,
            "functor_name": functor_name,
            "gl_name": cmd.name,
            "call_args_str": ", ".join(forwarded),
            "version_int": version_int,
        }

    def _generate_handle_param_decl(
        self, param: CommandParam, command: Command, api: str
    ) -> str:
        """Like ``_generate_dsa_param_decl`` but substitutes handle-valued
        parameters with the ``::{api}::handle::*`` wrapper type so handle
        classes reference each other consistently (addresses the TODO about
        handle:: methods preferring handle:: types over raw handle ids).
        Pointer params keep their raw handle-id type to preserve the ABI of
        the underlying GL array call.
        """
        if (
            param.class_
            and _is_uint_handle(param)
            and param.class_ in self._handle_classes
            and not param.is_pointer
        ):
            wrapper = _to_handle_name(param.class_)
            return f"::{api}::handle::{wrapper} {param.name}"
        return self._generate_dsa_param_decl(param, command, api)

    def _handle_method_call_arg(self, param: CommandParam) -> str:
        """Forward a handle-class method parameter to the free functor.

        Handle-valued wrapper parameters are unwrapped via ``.id()`` so the
        free functor sees its expected strong ``gl::`` handle type; every other
        parameter is forwarded by name (the functor's own operator() performs
        any ``.id`` / ``.loc`` / ``.value()`` extraction internally).
        """
        if (
            param.class_
            and _is_uint_handle(param)
            and param.class_ in self._handle_classes
            and not param.is_pointer
        ):
            return f"{param.name}.id()"
        return param.name

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
            name = name[len(class_camel) :]
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
        return {
            "group_name": clean_name,
            "base_type": base_type,
            "entries": entries,
            "is_bitmask": is_bitmask,
        }

    def _append_command_function_contexts(self, functions: list, command: Command) -> None:
        """Append every functor variant glaze emits for ``command`` to ``functions``.

        Mirrors the per-command logic that used to live inline in ``generate``:
        the canonical context, the std::string overload, the ArrayView overload,
        the scalar-return query overload, the InfoLog self-query overload, and
        the singular create/delete convenience overloads. Used by both the
        core-command pass and the extension-command pass so they stay in sync.
        """
        functions.append(self._function_context(command))
        string_param = _find_string_output_param(command)
        if string_param:
            length_param = _find_length_param(command)
            functions.append(self._string_overload_context(command, string_param, length_param))
        scalar_param = _find_scalar_query_param(command)
        if scalar_param is not None:
            functions.append(self._scalar_query_overload_context(command, scalar_param))
        if command.name in _INFOLOG_SELF_QUERY_MAP:
            functions.append(self._infolog_selfquery_overload_context(command))
        upload_params = _find_data_upload_params(command)
        if upload_params:
            data_param, size_param = upload_params
            functions.append(self._array_view_overload_context(command, data_param, size_param))
        creation_params = _find_object_creation_params(command)
        if creation_params:
            count_param, output_param = creation_params
            if output_param.class_ in self._handle_classes:
                functions.append(
                    self._single_object_creation_context(command, count_param, output_param)
                )
        deletion_params = _find_object_deletion_params(command)
        if deletion_params:
            count_param, input_param = deletion_params
            if input_param.class_ in self._handle_classes:
                functions.append(
                    self._single_object_deletion_context(command, count_param, input_param)
                )

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

    def _scalar_query_overload_context(
        self, command: Command, scalar_param: CommandParam
    ) -> dict:
        """Build an overload that drops a trailing scalar output pointer and returns it.

        Turns ``glGetShaderiv(shader, pname, params)`` into a second
        ``operator()(Shader shader, ShaderParameterName pname) -> GLint`` that
        fills a local and returns it, so callers can write
        ``GLint ok = gl::getShaderiv(shader, ShaderParameterName::eCompileStatus)``
        without wrangling an out-parameter.
        """
        func_name = self._convert_function_name(command.name)
        overload_params = [p for p in command.params if p is not scalar_param]
        params_str = ", ".join(self._generate_param_decl(p, command) for p in overload_params)

        call_parts = []
        for p in command.params:
            if p is scalar_param:
                call_parts.append("&result")
            else:
                call_parts.append(self._generate_call_arg(p, command))
        call_args_str = ", ".join(call_parts)

        body = (
            f"{scalar_param.type} result = 0;\n"
            f"::{command.name}({call_args_str});\n"
            "return result;"
        )
        return {
            "return_type": scalar_param.type,
            "func_name": func_name,
            "params_str": params_str,
            "gl_name": command.name,
            "body": body,
        }

    def _infolog_selfquery_overload_context(self, command: Command) -> dict:
        """Build a zero-extra-argument std::string overload for InfoLog commands.

        Generates an overload that keeps only the leading handle parameter and
        internally queries GL_INFO_LOG_LENGTH via the paired ``glGet*iv``
        function, sizes an std::string accordingly, and returns it. This is
        the convenience most callers actually want when diagnosing shader
        compile / program link failures.
        """
        iv_query = _INFOLOG_SELF_QUERY_MAP[command.name]
        func_name = self._convert_function_name(command.name)
        handle_param = command.params[0]
        handle_call = self._generate_call_arg(handle_param, command)
        params_str = self._generate_param_decl(handle_param, command)

        body = (
            "GLint length = 0;\n"
            f"{iv_query}({handle_call}, GL_INFO_LOG_LENGTH, &length);\n"
            "if (length <= 0) return std::string{};\n"
            "std::string result(static_cast<std::size_t>(length), '\\0');\n"
            "GLsizei written = 0;\n"
            f"::{command.name}({handle_call}, length, &written, &result[0]);\n"
            "result.resize(static_cast<std::size_t>(written));\n"
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
        extra_params = [p for p in command.params if p is not count_param and p is not output_param]
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

        body = f"{handle_name} obj;\n::{command.name}({call_args_str});\nreturn obj;"
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
        extra_params = [p for p in command.params if p is not count_param and p is not input_param]
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
        suffix = command.name[len("glCreate") :]
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

    def _generate_dsa_param_decl(self, param: CommandParam, command: Command, api: str) -> str:
        """Like _generate_param_decl but fully qualifies types that could collide in the dsa namespace."""
        needs_qualify = (
            param.class_ and _is_uint_handle(param) and param.class_ in self._handle_classes
        ) or (param.has_group() and param.group in self._emitted_groups)
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
                use_type = (
                    f"Flags<{clean}>"
                    if not namespace_prefix
                    else f"{namespace_prefix}Flags<{namespace_prefix}{clean}>"
                )
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
