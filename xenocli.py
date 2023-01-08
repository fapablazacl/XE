import subprocess
import os
import argparse
import json

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

class Coverage:
    def __init__(self, cm_build_dir) -> None:
        self.cm_build_dir = cm_build_dir

    def check_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            profile_data_file = self._merge_raw_profile(executable_test)
            report = self._get_data_profile_report(executable_test, profile_data_file)

            print(report)

    def _get_ctest_json_info(self):
        cmd = "ctest --show-only=json-v1"
        result = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.cm_build_dir)

        content = ""
        for line in result.stdout.readlines():
            content += line.decode()

        return json.loads(content)

    def _filter_ctest_tests(self, test_info):
        results = []

        for test in test_info["tests"]:
            commands = test["command"]

            for command in commands:
                results.append(command)

        return results

    def _merge_raw_profile(self, executable_test) -> str:
        parent_path = os.path.dirname(executable_test)

        profraw_file = os.path.join(parent_path, "default.profraw")
        profdata_file = os.path.join(parent_path, "default.profdata")

        cmd = "xcrun llvm-profdata merge {} --instr --sparse=true -o {}"
        cmd = cmd.format(profraw_file, profdata_file)

        os.system(cmd)

        return profdata_file

    def _get_data_profile_report(self, executable_test, profdata_file) -> str:
        cmd = "xcrun llvm-cov report {} -instr-profile={} -arch=x86_64"
        cmd = cmd.format(executable_test, profdata_file)

        result = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.cm_build_dir)

        content = ""
        for line in result.stdout.readlines():
            content += line.decode()

        return content


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command")

    args = parser.parse_args()

    if args.command == "format":
        formatter = Formatter(checker=ExtensionChecker())
        formatter.format(folder = "src", exclude="dependencies")
    elif args.command == "coverage":
        coverage = Coverage(cm_build_dir="coverage/debug")
        coverage.check_coverage()

if __name__=="__main__":
    main()
