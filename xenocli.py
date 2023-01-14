import subprocess
import os
import argparse
import json
import platform

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

    def show_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            module_path = os.curdir
            profile_data_file = self._merge_raw_profile(executable_test)

            llvm_terminal_report = self._run_command(self._get_data_profile_report_cmd(executable_test, profile_data_file), raise_on_error=True, cwd=self.cm_build_dir)
            print(llvm_terminal_report)

    def check_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            module_path = os.curdir
            profile_data_file = self._merge_raw_profile(executable_test)
            llvm_json_report = self._run_command(self._export_coverage_summary_json(executable_test, profile_data_file, module_path, "x86_64", "text"), raise_on_error=True)

            if not self._check_llvm_coverage_json_export(llvm_json_report, 90):
                print("Test \"{}\" doesn't pass minimum coverage threshold (90%)".format(executable_test))
                exit(1)

    def _check_llvm_coverage_json_export(self, report, minimum_coverage):
        llvm_json_type = "llvm.coverage.json.export"

        report_dict = json.loads(report)

        if "type" not in report_dict or report_dict["type"] != llvm_json_type:
            raise RuntimeError("Invalid LLVM JSON Document Type")

        for element in report_dict["data"]:
            for file in element["files"]:
                for category in ["branches", "functions", "instantiations", "lines", "regions"]:
                    summary = file["summary"]

                    count = summary[category]["count"]
                    covered = summary[category]["covered"]
                    percent = summary[category]["percent"]

                    if percent < minimum_coverage:
                        return False

        return True


    def _run_command(self, cmd, raise_on_error = False, cwd=None):
        process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=cwd)

        stdout = ""
        for line in process.stdout.readlines():
            stdout += line.decode()

        if process.stderr is None:
            stderr = None
        else:
            stderr = ""
            for line in process.stderr.readlines():
                stderr += line.decode()

        process.communicate()[0]

        exit_code = process.returncode

        if not raise_on_error:
            return (exit_code, stdout, stderr)
        else:
            if exit_code == 0:
                return stdout
            
            raise RuntimeError(stderr)


    def _get_ctest_json_info(self):
        cmd = "ctest --show-only=json-v1"

        exit_code, stdout, stderr = self._run_command(cmd, cwd=self.cm_build_dir)

        return json.loads(stdout)

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

        cmd = self._adjust_llvm_cmd("llvm-profdata merge {} --instr --sparse=true -o {}")
        cmd = cmd.format(profraw_file, profdata_file)

        exit_code = os.system(cmd)

        return profdata_file


    def _get_data_profile_report_cmd(self, executable_test, profdata_file) -> str:
        cmd = self._adjust_llvm_cmd("llvm-cov report {} -instr-profile={} -arch=x86_64")
        cmd = cmd.format(executable_test, profdata_file)
        return cmd

    def _get_coverage_show_cmd(self, exxecutable_test, profdata_file, module_path, output_dir):
        export_format = "html"
        arch = "x86_64"
        
        cmd = self._adjust_llvm_cmd("llvm-cov show {} -instr-profile={} -format={} -show-branches=percent -show-line-counts-or-regions -show-expansions -arch={} {} -output-dir={}")
        cmd = cmd.format(exxecutable_test, profdata_file, export_format, arch, module_path, output_dir)

        return cmd

    def _export_coverage_summary_json(self, executable_test, profdata_file, module_path, arch, format):
        cmd = "llvm-cov export -instr-profile={} {} -arch={} {} -format={} -summary-only"
        cmd = cmd.format(profdata_file, executable_test, arch, module_path, format)

        return cmd

    def _get_current_os(self):
        return platform.system()

    def _adjust_llvm_cmd(self, cmd):
        if self._get_current_os() == "Darwin":
            return "xcrun " + cmd
        
        return cmd


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
