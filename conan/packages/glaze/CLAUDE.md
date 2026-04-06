# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Purpose

glaze is a **code generator** packaged as a Conan package that produces language-specific headers and source files for the OpenGL API from the official Khronos XML registry (`OpenGL-Registry/xml/gl.xml`). It supports GL 1.0–4.6 and GLES 1.0–3.2.

Both C and C++11 bindings are generated.

See @README.md for more information.

## Architecture

```
glaze/
  parser.py              — parses gl.xml into the Registry data model
  model.py               — Registry, Feature, Command, Enum, Type dataclasses
  generators/
    base.py              — abstract Generator base class; owns the shared Jinja2 environment
    c_generator.py       — renders C header + source via templates/c/
    cpp_generator.py     — renders C++ header via templates/cpp/
  templates/
    c/gl.h.j2            — Jinja2 template for the C header
    c/gl.c.j2            — Jinja2 template for the C source
    cpp/gl.hpp.j2        — Jinja2 template for the C++ header
  utils/
    string_utils.py      — CamelCase splitting and capitalization helpers
glaze_cli.py             — CLI entry point (subcommands: generate, list-apis)
conanfile.py             — Conan package recipe (clones registry, runs generator, packages output)
pyproject.toml           — project metadata, ruff/mypy/pytest configuration
tests/
  conftest.py            — shared mini-XML fixture for all tests
  test_model.py          — dataclass and Registry method tests
  test_parser.py         — XML parsing tests
  test_c_generator.py    — C generator output tests
  test_cpp_generator.py  — C++ generator helper and output tests
  test_string_utils.py   — string utility tests
  test_cli.py            — CLI argument parsing tests
```

Generators inherit from `Generator` (ABC) and implement `generate(api, version) -> dict[str, str]`.
Output rendering is done exclusively through Jinja2 templates; no inline string building in generator code.

## Commands

### Generate code

```bash
# Generate C header + source
python3 glaze_cli.py generate --api gl 3.3 --lang c --output-dir out/

# Generate C++ header
python3 glaze_cli.py generate --api gl 3.3 --lang cpp --output-dir out/

# Generate both languages in one invocation (--lang is repeatable)
python3 glaze_cli.py generate --api gl 4.6 --api gles1 1.0 --lang c --lang cpp --output-dir out/

# List all available APIs and versions from the registry
python3 glaze_cli.py list-apis
```

### Lint, format, and type-check

```bash
ruff check glaze/ glaze_cli.py
ruff format glaze/ glaze_cli.py
mypy glaze/ glaze_cli.py
```

### Run tests

```bash
pytest tests/ -v
```

### Build as Conan package

```bash
conan create . --build=missing -o "apis=gl:3.3" -o "language=cpp"
```

### Data model concepts
- **Feature** — an OpenGL version with `require` and `remove` lists
- **Command** — an OpenGL function with typed parameters
- **Enum** — a named constant grouped by type (generated code uses GL enum symbols, not raw values)
- **Type** — typedef or struct

## Tests

Unit tests live in `tests/` and use pytest with a mini-XML fixture (~100 lines) instead of the real 2.5 MB `gl.xml`.

| Test file | What it validates |
|-----------|------------------|
| `test_string_utils.py` | `is_capitalized`, `split_capitalized`, `camel_case` |
| `test_model.py` | Dataclasses (`TypeDecl`, `CommandParam`, `Command`), `Registry` methods (`consolidate`, `collect_features`, `available_apis`) |
| `test_parser.py` | XML parsing of types, enums, commands, features, extensions, and error cases |
| `test_c_generator.py` | C generator file output, enum defines, function typedefs, loader |
| `test_cpp_generator.py` | C++ helpers (`_to_handle_name`, `_EnumIdentifierConverter`, `_build_group_rename`), generator output |
| `test_cli.py` | Argument parsing for `generate` and `list-apis` subcommands |
