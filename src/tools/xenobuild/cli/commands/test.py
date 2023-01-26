
from core.util import run_command

class TestCommand:
    @staticmethod
    def create(project):
        return TestCommand(project=project, cm_build_dir="coverage/debug")

    def __init__(self, project, cm_build_dir) -> None:
        self.project = project
        self.cm_build_dir = cm_build_dir

    def perform(self):
        self._test()

    def _test(self):
        cmd = "ctest --output-on-failure"

        try:
            output = run_command(cmd, raise_on_error=True, cwd=self.cm_build_dir)
            print(output)
        except RuntimeError:
            print("could not run the test command")
            print(cmd)
