#!/usr/bin/env python3
"""glaze_cli — unified OpenGL binding generator.

Usage examples:
  python glaze_cli.py generate --api gl --version 3.3 --lang cpp --output-dir tests/test-gl10
  python glaze_cli.py generate --api gl --version 3.3 --lang c   --output-dir tests/test-dynamicLoading
  python glaze_cli.py generate --api gl --version 4.6 --lang c --lang cpp --output-dir out/
  python glaze_cli.py list-apis
"""

import argparse
import os
import sys

from glaze.parser import RegistryParser
from glaze.generators.c_generator import CGenerator
from glaze.generators.cpp_generator import CppGenerator

DEFAULT_REGISTRY = os.path.join(
    os.path.dirname(__file__), "OpenGL-Registry", "xml", "gl.xml"
)

GENERATORS = {
    "c": CGenerator,
    "cpp": CppGenerator,
}


def cmd_generate(args):
    registry = _load_registry(args.registry)

    if not args.lang:
        print("error: at least one --lang must be specified", file=sys.stderr)
        sys.exit(1)

    unknown = [l for l in args.lang if l not in GENERATORS]
    if unknown:
        print(f"error: unknown language(s): {unknown}. Choose from: {list(GENERATORS.keys())}", file=sys.stderr)
        sys.exit(1)

    output_dir = args.output_dir or "."
    os.makedirs(output_dir, exist_ok=True)

    for lang in args.lang:
        gen = GENERATORS[lang](registry)
        try:
            files = gen.generate(args.api, args.version)
        except ValueError as e:
            print(f"error: {e}", file=sys.stderr)
            sys.exit(1)

        for rel_path, content in files.items():
            out_path = os.path.join(output_dir, rel_path)
            os.makedirs(os.path.dirname(out_path), exist_ok=True)
            with open(out_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  [{lang}] wrote {out_path}")


def cmd_list_apis(args):
    registry = _load_registry(args.registry)
    apis = registry.available_apis()
    print("Available APIs and versions:")
    for api in sorted(apis):
        versions = ", ".join(apis[api])
        print(f"  {api}: {versions}")


def _load_registry(registry_path: str):
    if not os.path.isfile(registry_path):
        print(f"error: registry file not found: {registry_path}", file=sys.stderr)
        sys.exit(1)
    print(f"Parsing {registry_path} ...")
    parser = RegistryParser()
    return parser.parse_file(registry_path)


def build_parser() -> argparse.ArgumentParser:
    root = argparse.ArgumentParser(
        prog="glaze_cli",
        description="Generate OpenGL C/C++ bindings from the Khronos XML registry.",
    )
    sub = root.add_subparsers(dest="command")
    sub.required = True

    # --- generate subcommand ---
    gen_p = sub.add_parser("generate", help="Generate binding files.")
    gen_p.add_argument(
        "--registry",
        default=DEFAULT_REGISTRY,
        help=f"Path to gl.xml (default: {DEFAULT_REGISTRY})",
    )
    gen_p.add_argument(
        "--api",
        required=True,
        help="GL API to target: gl, gles1, gles2, glsc2",
    )
    gen_p.add_argument(
        "--version",
        required=True,
        help="Version number, e.g. 3.3 or 4.6",
    )
    gen_p.add_argument(
        "--lang",
        action="append",
        metavar="LANG",
        help=f"Output language (repeatable). Choices: {', '.join(GENERATORS)}",
    )
    gen_p.add_argument(
        "--output-dir",
        default=".",
        help="Directory to write generated files into (default: current dir)",
    )
    gen_p.set_defaults(func=cmd_generate)

    # --- list-apis subcommand ---
    list_p = sub.add_parser("list-apis", help="List available APIs and versions.")
    list_p.add_argument(
        "--registry",
        default=DEFAULT_REGISTRY,
        help=f"Path to gl.xml (default: {DEFAULT_REGISTRY})",
    )
    list_p.set_defaults(func=cmd_list_apis)

    return root


def main():
    parser = build_parser()
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
