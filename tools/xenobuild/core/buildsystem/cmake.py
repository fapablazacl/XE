
import os

class CMakeDefinition:
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def evaluate(self):
        value = self.value

        return '-D' + self.name + ('' if value is None else ('=' + value))


class CMakeBuildConfig:
    def __init__(self, buildType, buildPath, installPath = None, prefixPaths = None, definitions = None) -> None:
        self.buildType = buildType
        self.buildPath = buildPath
        self.installPath = installPath
        self.prefixPaths = prefixPaths
        self.definitions = definitions


class CMakeProject:
    def __init__(self, sourcePath, build_prefix):
        self.sourcePath = sourcePath
        self._build_prefix = build_prefix

        self._build_type_dict = {
            "debug": {
                "type": "Debug",
                "path": "debug"
            },
            "release": {
                "type": "Release",
                "path": "release"
            }
        }

        self._build_config_dict = {
            "build": {
                "path": "build",
                "definitions": {
                    "XE_DEV_WARNINGS_AS_ERRORS": True,
                    "XE_DEV_LINT_CLANG_TIDY" : False,
                    "XE_DEV_UNIT_TEST" : False,
                    "XE_DEV_INSTRUMENT_COVERAGE" : False
                }
            },
            "lint": {
                "path": "lint",
                "definitions": {
                    "XE_DEV_WARNINGS_AS_ERRORS": False,
                    "XE_DEV_LINT_CLANG_TIDY" : True,
                    "XE_DEV_UNIT_TEST" : False,
                    "XE_DEV_INSTRUMENT_COVERAGE" : False
                }
            },
            "coverage": {
                "path": "coverage",
                "definitions": {
                    "XE_DEV_WARNINGS_AS_ERRORS": False,
                    "XE_DEV_LINT_CLANG_TIDY" : False,
                    "XE_DEV_UNIT_TEST" : True,
                    "XE_DEV_INSTRUMENT_COVERAGE" : True
                }
            }
        }

    def configure(self, name = None):
        configurations = [name]

        if name is None:
            configurations = [x for x in self._build_config_dict.keys()]

        for build_type_key in self._build_type_dict:
            build_path = self._build_type_dict[build_type_key]["path"]
            build_type = self._build_type_dict[build_type_key]["type"]

            for config_name in configurations:
                config = self._build_config_dict[config_name]

                build_config_path = os.path.join(self._build_prefix, build_path, config["path"])

                definition_dict = {}

                for key in config["definitions"].keys():
                    value = config["definitions"][key]

                    if type(value) == bool:
                        definition_dict[key] = "ON" if value else "OFF"
                    else:
                        definition_dict[key] = value

                self._configure(
                    buildType=build_type, 
                    buildPath=build_config_path, 
                    installPath = None, 
                    prefixPaths = None, 
                    definitions = definition_dict)

    def _createDefinition(self, name, value):
        return '-D' + name + '=' + value

    def _configure(self, buildType, buildPath, installPath = None, prefixPaths = None, definitions = None):
        os.system("mkdir -p " + buildPath)

        command_parts = [
            'cmake ',
            self.sourcePath, 
            '-G "Unix Makefiles"',
            # '-DCMAKE_OSX_ARCHITECTURES="arm64"',
            '-DCMAKE_BUILD_TYPE=' + buildType
        ]

        if installPath is not None:
            command_parts.append('-DCMAKE_INSTALL_PREFIX=' + installPath)

        if prefixPaths is not None:
            command_parts.append('-DCMAKE_PREFIX_PATH="' + ';'.join(prefixPaths) + '"')

        if definitions is not None:
            for key in definitions:
                definition = self._createDefinition(key, definitions[key])
                command_parts.append(definition)

        cmakeCommand = " ".join(command_parts)

        os.system("cd " + buildPath + ' && ' + cmakeCommand)

    def _build(self, buildPath):
        os.system("cd " + buildPath + ' && ' + "make")

    def _install(self, buildPath):
        os.system("cd " + buildPath + ' && ' + "make install")
