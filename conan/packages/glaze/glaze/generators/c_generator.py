from glaze.doc_parser import FunctionDoc
from glaze.generators.base import Generator
from glaze.model import Command, CommandParam, Enum, Feature, Registry
from glaze.utils.string_utils import version_to_int


class CGenerator(Generator):
    """Generates C dynamic-loading bindings.

    Output files:
      include/glaze/gl.h — type definitions, enum macros, function pointer
                            typedefs and extern declarations, loader prototype.
      src/gl.c            — function pointer variable definitions and a single
                            glazeLoadFunctions() implementation.
    """

    def __init__(
        self,
        registry: Registry,
        doc_index: dict[str, FunctionDoc] | None = None,
        extension_vendors: list[str] | None = None,
        extension_names: list[str] | None = None,
    ):
        super().__init__(registry, doc_index, extension_vendors, extension_names)

    @property
    def name(self) -> str:
        return "c"

    def generate(self, api: str, version: str) -> dict[str, str]:
        self._check_api_version(api, version)
        features = self.registry.collect_features(api, version)
        ext_emissions = self._collect_extension_emissions(api, version)
        type_name_set = self._collect_param_types(features, ext_emissions)

        return {
            f"include/glaze/{api}.h": self._render_template(
                "c/gl.h.j2",
                self._header_context(features, ext_emissions, type_name_set, api, version),
            ),
            f"src/{api}.c": self._render_template(
                "c/gl.c.j2", self._source_context(features, ext_emissions, api, version)
            ),
        }

    # ------------------------------------------------------- type collection

    def _collect_command_types(self, command: Command, type_name_set: set) -> None:
        """Add the GL type names referenced by a command's return type and parameters."""
        if command.return_type.name not in ("void",):
            type_name_set.add(command.return_type.name)
        for param in command.params:
            if param.data_type is not None:
                type_name_set.add(param.data_type)

    def _collect_param_types(self, features: list[Feature], ext_emissions: list[dict]) -> set:
        """Collect the set of GL type names used by command parameters and return types.

        Walks both core features and the extension emissions so extension-only
        commands (e.g. ``glUniform3ui64NV`` referencing ``GLuint64EXT``) get the
        typedefs they need declared in ``gl.h``.
        """
        type_name_set: set = set()
        for feature in features:
            for require in feature.require_list:
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    self._collect_command_types(command, type_name_set)
        for ext in ext_emissions:
            for command in ext["commands"]:
                self._collect_command_types(command, type_name_set)
        return type_name_set

    # --------------------------------------------------------------- contexts

    def _header_context(
        self,
        features: list[Feature],
        ext_emissions: list[dict],
        type_name_set: set,
        api: str,
        version: str,
    ) -> dict:
        # Collect all required type names: from command params/returns and <require><type> entries
        all_type_names: set = set(type_name_set)
        for feature in features:
            for require in feature.require_list:
                for type_ref in require.types:
                    all_type_names.add(type_ref.name)

        # Emit in registry order (gl.xml declaration order) to respect dependencies
        types = []
        for t in self.registry.types_list:
            if t.name in all_type_names:
                types.append(t.c_definition)

        feature_list = []
        for feature in features:
            enums = []
            commands = []
            for require in feature.require_list:
                for enum_ref in require.enums:
                    enum = self.registry.enum_by_name.get(enum_ref.name)
                    if enum is not None:
                        enums.append({"name": enum.name, "value": enum.value})
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    commands.append(
                        self._build_command_header_entry(
                            command, f" [{api.upper()} {feature.number}]"
                        )
                    )
            ver_int = version_to_int(feature.number)
            feature_list.append(
                {
                    "name": feature.name,
                    "version_int": ver_int,
                    "version_major": ver_int // 10,
                    "version_minor": ver_int % 10,
                    "enums": enums,
                    "commands": commands,
                }
            )

        # Extension declarations (deduped against the consolidated core set).
        extension_list = self._build_extension_header_list(ext_emissions)

        return {
            "types": types,
            "features": feature_list,
            "extensions": extension_list,
            "api": api,
            "version_floor_int": version_to_int(version),
            "generation_header": self._generation_header(api, version, "C"),
        }

    def _source_context(
        self, features: list[Feature], ext_emissions: list[dict], api: str, version: str
    ) -> dict:
        feature_list = []
        loader_feature_list = []
        debug_wrapper_features = []

        for feature in features:
            ver_int = version_to_int(feature.number)
            definitions = []
            entries = []
            wrappers = []
            for require in feature.require_list:
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    raw_name = f"glaze_{command.name}"
                    ptr_type = self._command_ptr_type_name(command.name)
                    definitions.append(f"GLAZE_API {ptr_type} {raw_name};")
                    entries.append(
                        {
                            "ptr_var": raw_name,
                            "ptr_type": ptr_type,
                            "gl_name": command.name,
                        }
                    )
                    wrappers.append(self._generate_debug_wrapper_context(command))
            feature_list.append(
                {
                    "name": feature.name,
                    "version_int": ver_int,
                    "definitions": definitions,
                }
            )
            loader_feature_list.append(
                {
                    "name": feature.name,
                    "version_int": ver_int,
                    "entries": entries,
                }
            )
            debug_wrapper_features.append(
                {
                    "name": feature.name,
                    "version_int": ver_int,
                    "wrappers": wrappers,
                }
            )

        extension_list = self._build_extension_source_list(ext_emissions)

        return {
            "features": feature_list,
            "loader_features": loader_feature_list,
            "debug_wrapper_features": debug_wrapper_features,
            "extensions": extension_list,
            "api": api,
            "version_floor_int": version_to_int(version),
            "generation_header": self._generation_header(api, version, "C"),
        }

    # ----------------------------------------------------------- shared builders

    def _build_command_header_entry(self, command: Command, ver_tag: str) -> dict:
        """Build the per-command dict consumed by ``gl.h.j2``.

        Used by both the per-feature loop and the extension loop. ``ver_tag``
        is the bracketed marker appended to the doc brief (e.g. ``" [GL 3.3]"``
        for a core feature, or ``" [GL_ARB_buffer_storage]"`` for an extension).
        """
        doc = self.doc_index.get(command.name)
        doc_brief = f"{doc.brief}{ver_tag}" if doc else ver_tag.strip()
        ptr_type = self._command_ptr_type_name(command.name)
        raw_name = f"glaze_{command.name}"
        debug_name = f"glaze_debug_{command.name}"
        return {
            "typedef": self._generate_command_ptr_typedef(command),
            "extern": self._generate_command_ptr_extern(command),
            "extern_raw": f"extern GLAZE_API {ptr_type} {raw_name};",
            "debug_decl": self._generate_debug_wrapper_decl(command),
            "debug_name": debug_name,
            "release_alias": f"#define {command.name} {raw_name}",
            "debug_alias": f"#define {command.name} {debug_name}",
            "name": command.name,
            "doc_brief": doc_brief,
            "doc_params": doc.params if doc else {},
        }

    def _build_extension_header_list(self, emissions: list[dict]) -> list[dict]:
        """Convert extension emissions into the dicts consumed by ``gl.h.j2``."""
        result: list[dict] = []
        for ext in emissions:
            enums = [{"name": e.name, "value": e.value} for e in ext["enums"]]
            commands = [
                self._build_command_header_entry(cmd, f" [{ext['name']}]")
                for cmd in ext["commands"]
            ]
            result.append(
                {
                    "name": ext["name"],
                    "short_name": ext["short_name"],
                    "guard_macro": ext["guard_macro"],
                    "flag_var": ext["flag_var"],
                    "khronos_define": ext["khronos_define"],
                    "enums": enums,
                    "commands": commands,
                }
            )
        return result

    def _build_extension_source_list(self, emissions: list[dict]) -> list[dict]:
        """Convert extension emissions into the dicts consumed by ``gl.c.j2``."""
        result: list[dict] = []
        for ext in emissions:
            definitions = []
            entries = []
            wrappers = []
            for cmd in ext["commands"]:
                raw_name = f"glaze_{cmd.name}"
                ptr_type = self._command_ptr_type_name(cmd.name)
                definitions.append(f"GLAZE_API {ptr_type} {raw_name};")
                entries.append(
                    {
                        "ptr_var": raw_name,
                        "ptr_type": ptr_type,
                        "gl_name": cmd.name,
                    }
                )
                wrappers.append(self._generate_debug_wrapper_context(cmd))
            result.append(
                {
                    "name": ext["name"],
                    "short_name": ext["short_name"],
                    "guard_macro": ext["guard_macro"],
                    "flag_var": ext["flag_var"],
                    "definitions": definitions,
                    "entries": entries,
                    "wrappers": wrappers,
                }
            )
        return result

    # ----------------------------------------------------------- command helpers

    def _command_ptr_type_name(self, command_name: str) -> str:
        return f"PFN{command_name.upper()}PROC"

    def _generate_command_ptr_typedef(self, command: Command) -> str:
        return_type_str = command.return_type.to_c_string()
        params_str = ", ".join(self._generate_param(p) for p in command.params)
        ptr_type = self._command_ptr_type_name(command.name)
        return f"typedef {return_type_str} (GLCALLCONV *{ptr_type})({params_str});"

    def _generate_command_ptr_extern(self, command: Command) -> str:
        return f"extern GLAZE_API {self._command_ptr_type_name(command.name)} {command.name};"

    def _generate_command_ptr_definition(self, command: Command) -> str:
        return f"GLAZE_API {self._command_ptr_type_name(command.name)} {command.name};"

    def _generate_param(self, param: CommandParam) -> str:
        type_str = " ".join(part for part in param.type_parts if part)
        return f"{type_str} {param.name}"

    def _generate_debug_wrapper_decl(self, command: Command) -> str:
        """Generate the debug wrapper function declaration."""
        return_type_str = command.return_type.to_c_string()
        params_str = ", ".join(self._generate_param(p) for p in command.params)
        if not params_str:
            params_str = "void"
        debug_name = f"glaze_debug_{command.name}"
        return f"GLAZE_API {return_type_str} GLCALLCONV {debug_name}({params_str});"

    def _generate_debug_wrapper_context(self, command: Command) -> dict:
        """Generate context for a debug wrapper function implementation."""
        return_type_str = command.return_type.to_c_string()
        is_void = return_type_str.strip() == "void"
        params_str = ", ".join(self._generate_param(p) for p in command.params)
        if not params_str:
            params_str = "void"
        args_str = ", ".join(p.name for p in command.params)
        raw_name = f"glaze_{command.name}"
        debug_name = f"glaze_debug_{command.name}"

        return {
            "debug_name": debug_name,
            "raw_name": raw_name,
            "return_type": return_type_str,
            "is_void": is_void,
            "params_str": params_str,
            "args_str": args_str,
            "param_count": len(command.params),
            "name": command.name,
        }

    def _generate_enum(self, enum: Enum) -> str:
        return f"#define {enum.name} {enum.value}"
