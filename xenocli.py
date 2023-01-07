
import os
import argparse

class ExtensionChecker:
    def __init__(self) -> None:
        self._extensionsSet = {
            ".c", ".cpp", ".cc", ".cxx", ".c++", 
            ".h", ".hpp", ".hh", ".hxx", ".h++", 
        }

    def check(self, filepath):
        file_name, file_ext = os.path.splitext(filepath)
        return file_ext in self._extensionsSet


class Formatter:
    def __init__(self, checker) -> None:
        self._checker = checker
        
    def _run_clang_format(self, source_file_path) -> None:
        # os.system("clang-format -i {}".format(source_file_path))
        print("clang-format -i {}".format(source_file_path))

    def format(self, folder, exclude) -> None:
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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command")

    args = parser.parse_args()

    if args.command == "format":
        formatter = Formatter(checker=ExtensionChecker())
        formatter.format(folder = "src", exclude="dependencies")

if __name__=="__main__":
    main()
