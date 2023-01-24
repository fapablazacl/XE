
import os
import subprocess


class ExtensionChecker:
    def __init__(self) -> None:
        self._extensionsSet = {
            ".c", ".cpp", ".cc", ".cxx", ".c++", 
            ".h", ".hpp", ".hh", ".hxx", ".h++", 
        }

    def check(self, filepath):
        file_name, file_ext = os.path.splitext(filepath)
        return file_ext in self._extensionsSet

def run_command(cmd, raise_on_error = False, cwd=None):
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
        
        raise RuntimeError(exit_code)
