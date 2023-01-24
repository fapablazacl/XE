
from core.util import run_command

class TestCommand:
    def __init__(self, cm_build_dir) -> None:
        self.cm_build_dir = cm_build_dir

    def test(self):
        cmd = "ctest --output-on-failure"

        try:
            output = run_command(cmd, raise_on_error=True, cwd=self.cm_build_dir)
            print(output)
        except RuntimeError:
            print("could not run the test command")
            print(cmd)
