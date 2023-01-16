
import argparse

from commands.coverage import Coverage
from commands.extension_checker import ExtensionChecker
from commands.formatter import Formatter

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
