
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
    def __init__(self, sourcePath):
        self.sourcePath = sourcePath
    
    def createDefinition(self, name, value):
        return '-D' + name + '=' + value

    def configure(self, buildType, buildPath, installPath = None, prefixPaths = None, definitions = None):
        os.system("mkdir -p " + buildPath)

        command_parts = [
            'cmake ',
            self.sourcePath, 
            '-G "Unix Makefiles"',
            '-DCMAKE_OSX_ARCHITECTURES="arm64"',
            '-DCMAKE_BUILD_TYPE=' + buildType
        ]

        if installPath is not None:
            command_parts.append('-DCMAKE_INSTALL_PREFIX=' + installPath)

        if prefixPaths is not None:
            command_parts.append('-DCMAKE_PREFIX_PATH="' + ';'.join(prefixPaths) + '"')

        if definitions is not None:
            for key in definitions:
                definition = self.createDefinition(key, definitions[key])
                command_parts.append(definition)

        cmakeCommand = " ".join(command_parts)

        os.system("cd " + buildPath + ' && ' + cmakeCommand)

    def build(self, buildPath):
        os.system("cd " + buildPath + ' && ' + "make")

    def install(self, buildPath):
        os.system("cd " + buildPath + ' && ' + "make install")
