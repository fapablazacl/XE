import os
import json
import platform

from core.util import run_command


class CoverageCliCommand:
    @staticmethod
    def create(project, build_dir="coverage/debug"):
        if os.getenv('XB_BUILD_DIR') is not None:
            build_dir = os.getenv('XB_BUILD_DIR')
            print("Using build directory from env variable XB_BUILD_DIR: " + build_dir)

        return CoverageCliCommand(project=project, cm_build_dir=build_dir)

    def __init__(self, project, cm_build_dir) -> None:
        self.project = project
        self.cm_build_dir = cm_build_dir
        self.enableCheck = False

    def perform(self):
        self._check_coverage()

    def _show_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            module_path = os.curdir
            profile_data_file = self._merge_raw_profile(executable_test)

            llvm_terminal_report = self._run_command(
                self._get_data_profile_report_cmd(executable_test, profile_data_file), raise_on_error=True,
                cwd=self.cm_build_dir)
            print(llvm_terminal_report)

    def _check_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            module_path = os.curdir
            profile_data_file = self._merge_raw_profile(executable_test)
            llvm_json_report = self._run_command(
                self._export_coverage_summary_json(executable_test, profile_data_file, module_path, "x86_64", "text"),
                raise_on_error=True)

            if self.enableCheck and not self._check_llvm_coverage_json_export(llvm_json_report, 90):
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

    def _run_command(self, cmd, raise_on_error=False, cwd=None):
        return run_command(cmd, raise_on_error=raise_on_error, cwd=cwd)

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

        cmd = self._adjust_llvm_cmd(
            "llvm-cov show {} -instr-profile={} -format={} -show-branches=percent -show-line-counts-or-regions -show-expansions -arch={} {} -output-dir={}")
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
