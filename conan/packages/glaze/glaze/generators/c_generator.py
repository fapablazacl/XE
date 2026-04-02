from typing import Dict, List

from glaze.generators.base import Generator
from glaze.model import Registry, Feature, Command, Type, Enum


class CGenerator(Generator):
    """Generates C dynamic-loading bindings.

    Output files:
      include/glaze/gl.h — type definitions, enum macros, function pointer
                            typedefs and extern declarations, loader prototype.
      src/gl.c            — function pointer variable definitions and a single
                            glazeLoadFunctions() implementation.
    """

    def __init__(self, registry: Registry):
        super().__init__(registry)

    @property
    def name(self) -> str:
        return "c"

    def generate(self, api: str, version: str) -> Dict[str, str]:
        self._check_api_version(api, version)
        features = self.registry.collect_features(api, version)
        type_name_set = self._collect_param_types(features)

        return {
            "include/glaze/gl.h": self._render_template("c/gl.h.j2", self._header_context(features, type_name_set)),
            "src/gl.c": self._render_template("c/gl.c.j2", self._source_context(features)),
        }

    # ----------------------------------------------------------------- checks

    def _check_api_version(self, api: str, version: str):
        available = self.registry.available_apis()
        if api not in available:
            raise ValueError(f"API '{api}' not found. Available: {list(available.keys())}")
        if version not in available[api]:
            raise ValueError(f"Version '{version}' not found for '{api}'. Available: {available[api]}")

    # ------------------------------------------------------- type collection

    def _collect_param_types(self, features: List[Feature]) -> set:
        """Collect the set of GL type names used by command parameters."""
        type_name_set: set = set()
        for feature in features:
            for require in feature.require_list:
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    for param in command.params:
                        if param.data_type is not None:
                            type_name_set.add(param.data_type)
        return type_name_set

    # --------------------------------------------------------------- contexts

    def _header_context(self, features: List[Feature], type_name_set: set) -> dict:
        types = []
        for type_name in sorted(type_name_set):
            t = self.registry.type_by_name.get(type_name)
            if t is not None:
                types.append(t.c_definition)

        feature_list = []
        for feature in features:
            enums = []
            commands = []
            for require in feature.require_list:
                for type_ref in require.types:
                    t = self.registry.type_by_name.get(type_ref.name)
                    if t is not None:
                        types.append(t.c_definition)
                for enum_ref in require.enums:
                    enum = self.registry.enum_by_name.get(enum_ref.name)
                    if enum is not None:
                        enums.append({"name": enum.name, "value": enum.value})
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    commands.append({
                        "typedef": self._generate_command_ptr_typedef(command),
                        "extern": self._generate_command_ptr_extern(command),
                    })
            feature_list.append({"name": feature.name, "enums": enums, "commands": commands})

        return {"types": types, "features": feature_list}

    def _source_context(self, features: List[Feature]) -> dict:
        feature_list = []
        loader_entries = []

        for feature in features:
            definitions = []
            for require in feature.require_list:
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    definitions.append(self._generate_command_ptr_definition(command))
                    loader_entries.append({
                        "ptr_var": command.name,
                        "ptr_type": self._command_ptr_type_name(command.name),
                        "gl_name": command.name,
                    })
            feature_list.append({"name": feature.name, "definitions": definitions})

        return {"features": feature_list, "loader_entries": loader_entries}

    # ----------------------------------------------------------- command helpers

    def _command_ptr_type_name(self, command_name: str) -> str:
        return f"PFN{command_name.upper()}PROC"

    def _generate_command_ptr_typedef(self, command: Command) -> str:
        return_type_str = command.return_type.to_c_string()
        params_str = ", ".join(self._generate_param(p) for p in command.params)
        ptr_type = self._command_ptr_type_name(command.name)
        return f"typedef GLAPI {return_type_str} (GLCALLCONV *{ptr_type})({params_str});"

    def _generate_command_ptr_extern(self, command: Command) -> str:
        return f"extern {self._command_ptr_type_name(command.name)} {command.name};"

    def _generate_command_ptr_definition(self, command: Command) -> str:
        return f"{self._command_ptr_type_name(command.name)} {command.name};"

    def _generate_param(self, param) -> str:
        type_str = " ".join(part for part in param.type_parts if part)
        return f"{type_str} {param.name}"

    def _generate_enum(self, enum: Enum) -> str:
        return f"#define {enum.name} {enum.value}"
