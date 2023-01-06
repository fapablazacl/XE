
import os
import sys

class Formatter:
    def __init__(self) -> None:
        pass
        
    def _run_clang_format(self, source_file_path) -> None:
        os.system("clang-format -i {}".format(source_file_path))

    def format(self, folder, exclude) -> None:
        for root, subdirs, files in os.walk(folder):
            if root == exclude:
                continue

            for file in files:
                file_path = os.path.join(root, file)
                self._run_clang_format(file_path)


command = sys.argv[1]

if command == "format":
    formatter = Formatter()

    folder = "src"
    excluded = "dependencies"
    formatter.format(folder, exclude=excluded)
