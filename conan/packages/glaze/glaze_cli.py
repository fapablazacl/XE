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

from glaze.doc_parser import parse_refpages
from glaze.generators.c_generator import CGenerator
from glaze.generators.cpp_generator import CppGenerator
from glaze.model import Registry
from glaze.parser import RegistryParser

DEFAULT_REGISTRY = os.path.join(os.path.dirname(__file__), "OpenGL-Registry", "xml", "gl.xml")
DEFAULT_REFPAGES = os.path.join(os.path.dirname(__file__), "OpenGL-Refpages")

GENERATORS = {
    "c": CGenerator,
    "cpp": CppGenerator,
}


def cmd_generate(args: argparse.Namespace) -> None:
    registry = _load_registry(args.registry)

    if not args.lang:
        print("error: at least one --lang must be specified", file=sys.stderr)
        sys.exit(1)

    unknown = [lang_name for lang_name in args.lang if lang_name not in GENERATORS]
    if unknown:
        print(
            f"error: unknown language(s): {unknown}. Choose from: {list(GENERATORS.keys())}",
            file=sys.stderr,
        )
        sys.exit(1)

    output_dir = args.output_dir or "."
    os.makedirs(output_dir, exist_ok=True)

    # Parse refpages documentation if available
    refpages_dir = getattr(args, "refpages", None)
    doc_indices: dict[str, dict] = {}
    if refpages_dir and os.path.isdir(refpages_dir):
        for api_name, _ in args.api:
            if api_name not in doc_indices:
                doc_indices[api_name] = parse_refpages(refpages_dir, api_name)
                print(f"  Parsed {len(doc_indices[api_name])} doc entries for {api_name}")

    for lang in args.lang:
        for api_name, api_version in args.api:
            doc_index = doc_indices.get(api_name, {})
            gen = GENERATORS[lang](registry, doc_index=doc_index)
            try:
                files = gen.generate(api_name, api_version)
            except ValueError as e:
                print(f"error: {e}", file=sys.stderr)
                sys.exit(1)

            for rel_path, content in files.items():
                out_path = os.path.join(output_dir, rel_path)
                os.makedirs(os.path.dirname(out_path), exist_ok=True)
                with open(out_path, "w", encoding="utf-8") as f:
                    f.write(content)
                print(f"  [{lang} {api_name}] wrote {out_path}")


def cmd_list_apis(args: argparse.Namespace) -> None:
    registry = _load_registry(args.registry)
    apis = registry.available_apis()
    print("Available APIs and versions:")
    for api in sorted(apis):
        versions = ", ".join(apis[api])
        print(f"  {api}: {versions}")


def _load_registry(registry_path: str) -> Registry:
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
        nargs=2,
        action="append",
        metavar=("API_NAME", "VERSION"),
        required=True,
        help="GL API to target and its version. Can be repeated. e.g. --api gl 3.3 --api gles1 1.0",
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
    gen_p.add_argument(
        "--refpages",
        default=DEFAULT_REFPAGES,
        help=f"Path to OpenGL-Refpages directory (default: {DEFAULT_REFPAGES})",
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


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
