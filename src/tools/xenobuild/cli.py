
import argparse

from commands.coverage import Coverage
from commands.util import ExtensionChecker
from commands.formatter import Formatter
from commands.test import TestCommand

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command")

    args = parser.parse_args()

    if args.command == "format":
        formatter = Formatter(checker=ExtensionChecker())
        formatter.format(folder = "src", exclude="dependencies")
    if args.command == "test":
        test_cmd = TestCommand(cm_build_dir="coverage/debug")
        test_cmd.test()
    elif args.command == "coverage":
        coverage = Coverage(cm_build_dir="coverage/debug")
        coverage.check_coverage()
    else:
        print("Invalid command \"{}\"".format(args.command))

if __name__=="__main__":
    main()
