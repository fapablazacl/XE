
from core.util import run_command

class ConfigureCliCommand:
    @staticmethod
    def create(project):
        return ConfigureCliCommand(project=project)

    def __init__(self, project) -> None:
        self.project = project

    def perform(self):
        self.project.configure(None)
