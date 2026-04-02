from typing import Dict, List

from glaze.generators.base import Generator
from glaze.model import Registry, Feature, Command, Type, Enum


class CGenerator(Generator):
    """Generates C dynamic-loading bindings.

    Output files:
      include/glaze/gl.h — type definitions, enum macros, function pointer
                            typedefs and extern declarations, loader prototype.
      src/gl.c            — function pointer variable definitions and a single
                            glaze_load_functions() implementation.
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
            "include/glaze/gl.h": self._generate_header(features, type_name_set),
            "src/gl.c": self._generate_source(features),
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

    # ----------------------------------------------------------------- header

    def _generate_header(self, features: List[Feature], type_name_set: set) -> str:
        code = self._header_prologue()

        code += "/* loader declarations */\n"
        code += "typedef void (*GLAZE_PROC)(void);\n"
        code += "typedef GLAZE_PROC (*GLAZE_GETPROCADDRESS)(const char *name);\n"
        code += "extern void glaze_load_functions(GLAZE_GETPROCADDRESS getProcAddress);\n\n"

        code += "/* data type definitions */\n"
        code += self._generate_types(type_name_set)
        code += "\n"

        for feature in features:
            code += self._generate_header_feature(feature)
            code += "\n"

        code += self._header_epilogue()
        return code

    def _header_prologue(self) -> str:
        return """\
#pragma once

#ifndef __GLAZE_GL_H__
#define __GLAZE_GL_H__

#include <KHR/khrplatform.h>

#if defined(__gl_h_) || defined(__GL_H__)
  #error please include this header instead
#endif

#define __gl_h_
#define __GL_H__

#if _WIN32
  #define GLAPI
  #define GLCALLCONV __stdcall
#else
  #define GLAPI
  #define GLCALLCONV
#endif

#if defined(__cplusplus)
extern "C" {
#endif

"""

    def _header_epilogue(self) -> str:
        return """\
#if defined(__cplusplus)
}
#endif

#endif
"""

    def _generate_types(self, type_name_set: set) -> str:
        code = ""
        for type_name in type_name_set:
            t = self.registry.type_by_name.get(type_name)
            if t is not None:
                code += f"{t.c_definition}\n"
        return code

    def _generate_header_feature(self, feature: Feature) -> str:
        code = f"/* {feature.name} definitions */\n"
        for require in feature.require_list:
            for type_ref in require.types:
                t = self.registry.type_by_name.get(type_ref.name)
                if t is not None:
                    code += f"{t.c_definition}\n"
            for enum_ref in require.enums:
                enum = self.registry.enum_by_name.get(enum_ref.name)
                if enum is not None:
                    code += f"{self._generate_enum(enum)}\n"
            for command_ref in require.commands:
                command = self.registry.command_by_name.get(command_ref.name)
                if command is None:
                    continue
                code += f"{self._generate_command_ptr_typedef(command)}\n"
                code += f"{self._generate_command_ptr_extern(command)}\n"
                code += "\n"
        return code

    # ----------------------------------------------------------------- source

    def _generate_source(self, features: List[Feature]) -> str:
        code = "#include <glaze/gl.h>\n\n"

        all_commands: List[Command] = []

        for feature in features:
            feature_commands: List[Command] = []
            code += f"/* {feature.name} function pointer variables */\n"
            for require in feature.require_list:
                for command_ref in require.commands:
                    command = self.registry.command_by_name.get(command_ref.name)
                    if command is None:
                        continue
                    code += f"{self._generate_command_ptr_definition(command)}\n"
                    feature_commands.append(command)
            code += "\n"
            all_commands.extend(feature_commands)

        code += self._generate_loader(all_commands)
        return code

    def _generate_loader(self, commands: List[Command]) -> str:
        code = "void glaze_load_functions(GLAZE_GETPROCADDRESS getProcAddress) {\n"
        for command in commands:
            ptr_type = self._command_ptr_type_name(command.name)
            ptr_var = command.name
            code += f'    {ptr_var} = ({ptr_type})getProcAddress("{command.name}");\n'
        code += "}\n"
        return code

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
        if param.is_void:
            return f'{"const " if param.is_const else ""}void{"*" * param.pointer_indirection} {param.name}'
        return f'{"const " if param.is_const else ""}{param.data_type}{"*" * param.pointer_indirection} {param.name}'

    def _generate_enum(self, enum: Enum) -> str:
        return f"#define {enum.name} {enum.value}"
