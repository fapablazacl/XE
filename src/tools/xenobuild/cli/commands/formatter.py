import os

from core.util import ExtensionChecker

class Formatter:
    @staticmethod
    def create(project):
        return Formatter(project=project, checker=ExtensionChecker())

    def __init__(self, project, checker) -> None:
        self._project = project
        self._checker = checker

    def perform(self):
        self._format(folder = "src", exclude="dependencies")
    
    def _run_clang_format(self, source_file_path) -> None:
        cmd = "clang-format -i {}".format(source_file_path)
        print(cmd)
        os.system(cmd)
        
    def _format(self, folder, exclude) -> None:
        for root, subdirs, files in os.walk(folder):
            if root.find(exclude) != -1:
                continue

            for file in files:
                if not self._is_file_formatteable(file):
                    continue

                file_path = os.path.join(root, file)
                self._run_clang_format(file_path)

    def _is_file_formatteable(self, file):
        return self._checker.check(file)
