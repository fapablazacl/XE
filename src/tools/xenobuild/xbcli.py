
import os
import argparse

from core.util import ExtensionChecker
from core.buildsystem.cmake import CMakeBuildConfig, CMakeProject, CMakeDefinition
from cli.commands.coverage import Coverage
from cli.commands.formatter import Formatter
from cli.commands.test import TestCommand

class XBProject:
    def __init__(self, name, path):
        self.name = name
        self.path = path
        self.cmakeProject = CMakeProject(self.path)

    def add_build_configuration(self):
        pass

    def set_current_build_configuration(self):
        pass


class CliApp:
    def __init__(self) -> None:
        self._commands = {
            "format": Formatter,
            "test": TestCommand,
            "coverage": Coverage
        }

        self.parser = argparse.ArgumentParser()
        self.parser.add_argument("command")

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

        command_class = self._commands
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
        print ("available commands:")

        for command_name in self._commands:
            print(command_name)

    def _handle_command(self, command_class, project):
        command = command_class.create()
        command.perform(project)


"""
def main():
    commands = {
        "format": Formatter,
        "test": TestCommand,
        "coverage": Coverage
    }

    parser = argparse.ArgumentParser()
    parser.add_argument("command")

    args = parser.parse_args()

    if not args.command:
        print ("available commands:")

        for command in commands:
            print(command)

        return 0
    
    commandName = args.command
    Command = commands[commandName]

    command = Command.create()

    if args.command == "format":
        formatter = Formatter(checker=ExtensionChecker())
        formatter.format(folder = "src", exclude="dependencies")
    if args.command == "test":
        test_cmd = TestCommand(cm_build_dir="coverage/debug")
        test_cmd.test()
    elif args.command == "coverage":
        coverage = Coverage(cm_build_dir="coverage/debug")
        coverage.check_coverage()
    else:
        print("Invalid command \"{}\"".format(args.command))
"""

if __name__=="__main__":
    ret_code = CliApp().run()
    exit(ret_code)
