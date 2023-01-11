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

    def check_coverage(self):
        test_info = self._get_ctest_json_info()
        executable_tests = self._filter_ctest_tests(test_info)

        for executable_test in executable_tests:
            profile_data_file = self._merge_raw_profile(executable_test)
            # report = self._get_data_profile_report(executable_test, profile_data_file)
            # print(report)

            cmd = self._get_coverage_show_cmd(executable_test, profile_data_file, "", "coverage")
            print(cmd)
            break


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

        cmd = self._adjust_llvm_cmd("llvm-profdata merge {} --instr --sparse=true -o {}")
        cmd = cmd.format(profraw_file, profdata_file)

        os.system(cmd)

        return profdata_file

    def _get_data_profile_report(self, executable_test, profdata_file) -> str:
        cmd = self._adjust_llvm_cmd("llvm-cov report {} -instr-profile={} -arch=x86_64")
        cmd = cmd.format(executable_test, profdata_file)

        print(cmd)

        result = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.cm_build_dir)

        content = ""
        for line in result.stdout.readlines():
            content += line.decode()

        return content

    def _get_coverage_show_cmd(self, exxecutable_test, profdata_file, module_path, output_dir):
        export_format = "html"
        arch = "x86_64"
        
        cmd = "xcrun llvm-cov show {} -instr-profile={} -format={} -show-branches=percent -show-line-counts-or-regions -show-expansions -arch={} {} -output-dir={}"
        cmd = cmd.format(exxecutable_test, profdata_file, export_format, arch, module_path, output_dir)

        return cmd

    def _get_current_os(self):
        return platform.system()

    def _adjust_llvm_cmd(self, cmd):
        if self._get_current_os() == "Darwin":
            return "xcrun " + cmd
        
        return cmd


def _parse_coverage_json_export():
    report = """
{
    "data": [
        {
            "files": [
                {
                    "filename": "/home/Desktop/nativedevcl/XE/src/engine/scene/src/xe/scene/Projection.h",
                    "summary": {
                        "branches": {
                            "count": 4,
                            "covered": 3,
                            "notcovered": 1,
                            "percent": 75
                        },
                        "functions": {
                            "count": 2,
                            "covered": 2,
                            "percent": 100
                        },
                        "instantiations": {
                            "count": 2,
                            "covered": 2,
                            "percent": 100
                        },
                        "lines": {
                            "count": 19,
                            "covered": 19,
                            "percent": 100
                        },
                        "regions": {
                            "count": 6,
                            "covered": 6,
                            "notcovered": 0,
                            "percent": 100
                        }
                    }
                },
                {
                    "filename": "/home/Desktop/nativedevcl/XE/src/engine/scene/src/xe/scene/Trackball.cpp",
                    "summary": {
                        "branches": {
                            "count": 2,
                            "covered": 0,
                            "notcovered": 2,
                            "percent": 0
                        },
                        "functions": {
                            "count": 8,
                            "covered": 6,
                            "percent": 75
                        },
                        "instantiations": {
                            "count": 8,
                            "covered": 6,
                            "percent": 75
                        },
                        "lines": {
                            "count": 20,
                            "covered": 6,
                            "percent": 30
                        },
                        "regions": {
                            "count": 11,
                            "covered": 6,
                            "notcovered": 5,
                            "percent": 54.54545454545454
                        }
                    }
                },
                {
                    "filename": "/home/Desktop/nativedevcl/XE/src/engine/scene/src/xe/scene/Trackball.h",
                    "summary": {
                        "branches": {
                            "count": 0,
                            "covered": 0,
                            "notcovered": 0,
                            "percent": 0
                        },
                        "functions": {
                            "count": 4,
                            "covered": 4,
                            "percent": 100
                        },
                        "instantiations": {
                            "count": 4,
                            "covered": 4,
                            "percent": 100
                        },
                        "lines": {
                            "count": 4,
                            "covered": 4,
                            "percent": 100
                        },
                        "regions": {
                            "count": 4,
                            "covered": 4,
                            "notcovered": 0,
                            "percent": 100
                        }
                    }
                },
                {
                    "filename": "/home/Desktop/nativedevcl/XE/src/engine/scene/src/xe/scene/VirtualSphere.cpp",
                    "summary": {
                        "branches": {
                            "count": 2,
                            "covered": 0,
                            "notcovered": 2,
                            "percent": 0
                        },
                        "functions": {
                            "count": 5,
                            "covered": 4,
                            "percent": 80
                        },
                        "instantiations": {
                            "count": 5,
                            "covered": 4,
                            "percent": 80
                        },
                        "lines": {
                            "count": 14,
                            "covered": 4,
                            "percent": 28.571428571428569
                        },
                        "regions": {
                            "count": 8,
                            "covered": 4,
                            "notcovered": 4,
                            "percent": 50
                        }
                    }
                },
                {
                    "filename": "/home/Desktop/nativedevcl/XE/src/engine/scene/src/xe/scene/VirtualSphere.h",
                    "summary": {
                        "branches": {
                            "count": 0,
                            "covered": 0,
                            "notcovered": 0,
                            "percent": 0
                        },
                        "functions": {
                            "count": 2,
                            "covered": 0,
                            "percent": 0
                        },
                        "instantiations": {
                            "count": 2,
                            "covered": 0,
                            "percent": 0
                        },
                        "lines": {
                            "count": 2,
                            "covered": 0,
                            "percent": 0
                        },
                        "regions": {
                            "count": 2,
                            "covered": 0,
                            "notcovered": 2,
                            "percent": 0
                        }
                    }
                }
            ],
            "totals": {
                "branches": {
                    "count": 8,
                    "covered": 3,
                    "notcovered": 5,
                    "percent": 37.5
                },
                "functions": {
                    "count": 21,
                    "covered": 16,
                    "percent": 76.19047619047619
                },
                "instantiations": {
                    "count": 21,
                    "covered": 16,
                    "percent": 76.19047619047619
                },
                "lines": {
                    "count": 59,
                    "covered": 33,
                    "percent": 55.932203389830505
                },
                "regions": {
                    "count": 31,
                    "covered": 20,
                    "notcovered": 11,
                    "percent": 64.516129032258064
                }
            }
        }
    ],
    "type": "llvm.coverage.json.export",
    "version": "2.0.1"
}
"""
    report = report.strip()
    lines = report.splitlines()

    for i in range(len(lines)):
        if i == 0:
            continue

        line = lines[i]
        
        if line.find("----------") != -1:
            continue

        parts = list(filter(lambda part: part != "", line.split(" ")))

        print(parts)



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
    _parse_coverage_report()
    # main()
