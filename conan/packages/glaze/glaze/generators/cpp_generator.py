from typing import Dict, List, Optional

from glaze.generators.base import Generator
from glaze.model import Registry, Command, CommandParam, Enum
from glaze.utils.string_utils import split_capitalized


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

    @property
    def name(self) -> str:
        return "cpp"

    def generate(self, api: str, version: str) -> Dict[str, str]:
        self._check_api_version(api, version)
        consolidated = self.registry.consolidate(api, version)

        # Collect enum groups referenced by the commands in this version
        group_set: set = set()
        for command_name in consolidated.commands:
            command = self.registry.command_by_name.get(command_name)
            if command is None:
                continue
            for param in command.params:
                if param.has_group() and param.group in self.registry.group_to_enums:
                    group_set.add(param.group)

        # Build enum class context — only include enums in the consolidated set
        enum_classes = []
        for group_name in sorted(group_set):
            all_enums = self.registry.group_to_enums[group_name]
            filtered = [e for e in all_enums if e.name in consolidated.enums]
            if filtered:
                enum_classes.append(self._enum_class_context(group_name, filtered))

        # Build inline function context
        functions = []
        for command_name in sorted(consolidated.commands):
            command = self.registry.command_by_name.get(command_name)
            if command is None:
                continue
            functions.append(self._function_context(command))

        filename = f"include/glaze/{api}.hpp"
        context = {"api": api, "enum_classes": enum_classes, "functions": functions}
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
        base_type = "GLboolean" if group_name == "Boolean" else "GLenum"
        converter = _EnumIdentifierConverter(group_name, self._capitalizer)
        entries = [{"name": converter.convert(e.name), "value": e.name} for e in enums]
        return {"group_name": group_name, "base_type": base_type, "entries": entries}

    def _function_context(self, command: Command) -> dict:
        return_type_str = command.return_type_str or command.return_type.to_c_string()
        func_name = self._convert_function_name(command.name)
        params_str = ", ".join(self._generate_param_decl(p) for p in command.params)
        call_args_str = ", ".join(self._generate_call_arg(p) for p in command.params)
        return {
            "return_type": return_type_str,
            "func_name": func_name,
            "params_str": params_str,
            "call_args_str": call_args_str,
            "gl_name": command.name,
        }

    # ----------------------------------------------------------- param helpers

    def _convert_function_name(self, gl_name: str) -> str:
        # glClearColor → clearColor
        return gl_name[2:3].lower() + gl_name[3:]

    def _generate_param_decl(self, param: CommandParam) -> str:
        return f"{self._param_type_str(param)} {param.name}"

    def _param_type_str(self, param: CommandParam, ignore_group: bool = False) -> str:
        """Reconstruct the parameter type string, substituting enum class name if applicable."""
        use_type = param.type
        if (
            not ignore_group
            and param.has_group()
            and param.group in self.registry.group_to_enums
        ):
            use_type = param.group

        parts = []
        for part in param.type_parts:
            if part == param.type:
                parts.append(use_type if use_type else part)
            elif part:
                parts.append(part)
        return " ".join(parts)

    def _generate_call_arg(self, param: CommandParam) -> str:
        """Generate the argument expression for the call to the underlying GL function."""
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
