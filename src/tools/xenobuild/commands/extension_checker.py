
import os

class ExtensionChecker:
    def __init__(self) -> None:
        self._extensionsSet = {
            ".c", ".cpp", ".cc", ".cxx", ".c++", 
            ".h", ".hpp", ".hh", ".hxx", ".h++", 
        }

    def check(self, filepath):
        file_name, file_ext = os.path.splitext(filepath)
        return file_ext in self._extensionsSet
