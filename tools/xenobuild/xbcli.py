import os
import argparse

from core.util import ExtensionChecker
from core.buildsystem.cmake import CMakeBuildConfig, CMakeProject, CMakeDefinition
from cli.commands.coverage import CoverageCliCommand
from cli.commands.formatter import FormatCliCommand
from cli.commands.test import TestCliCommand
from cli.commands.configure import ConfigureCliCommand


class XBProject:
    def __init__(self, name, path):
        self.name = name
        self.path = path
        self.cmake_project = CMakeProject(self.path, ".xenobuild/cmake")

    def configure(self, config_name):
        self.cmake_project.configure(config_name)

    def add_build_configuration(self):
        pass

    def set_current_build_configuration(self):
        pass


class CliApp:
    def __init__(self) -> None:
        self._commands = {
            "configure": ConfigureCliCommand,
            "format": FormatCliCommand,
            "test": TestCliCommand,
            "coverage": CoverageCliCommand
        }

        self.parser = argparse.ArgumentParser(description="Utility Wrapper for Plumbing C/C++ Projects")

        choices = [x for x in self._commands.keys()]
        self.parser.add_argument("command", choices=choices)

    def run(self):
        args = self._parse_args()

        command_name = args.command

        if command_name is None:
            self._handle_no_command()
            return 0

        if command_name not in self._commands:
            print("Unknown command \"{}\"".format(args.command))
            return 1

        project = self._open_project(None)

        command_class = self._commands[command_name]
        self._handle_command(command_class=command_class, project=project)

        return 0

    def _parse_args(self):
        return self.parser.parse_args()

    def _open_project(self, path):
        if path is None:
            path = os.getcwd()

        cmake_project_path = self._get_full_cmake_project_path(path=path)

        if not os.path.isfile(cmake_project_path):
            raise Exception("Current path doesn't have a CMake Project")

        name = self._get_cmake_project_name(path)

        return XBProject(name=name, path=path)

    def _get_full_cmake_project_path(self, path):
        return os.path.join(path, "CMakeLists.txt")

    def _get_cmake_project_name(self, cmake_project_path):
        return "project01"

    def _handle_no_command(self):
        print("available commands:")

        for command_name in self._commands:
            print(command_name)

    def _handle_command(self, command_class, project):
        command = command_class.create(project)
        command.perform()


if __name__ == "__main__":
    ret_code = CliApp().run()
    exit(ret_code)
