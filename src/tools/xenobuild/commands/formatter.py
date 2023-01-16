import os

class Formatter:
    def __init__(self, checker) -> None:
        self._checker = checker
        
    def _run_clang_format(self, source_file_path) -> None:
        cmd = "clang-format -i {}".format(source_file_path)
        print(cmd)
        os.system(cmd)
        
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