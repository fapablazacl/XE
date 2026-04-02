# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Purpose

glaze is a **code generator** that produces language-specific headers and source files for the OpenGL API from the official Khronos XML registry (`OpenGL-Registry/xml/gl.xml`). It supports GL 1.0–4.6 and GLES 1.0–3.2.

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
glaze_cli.py             — CLI entry point (subcommands: generate, list-apis)
```

Generators inherit from `Generator` (ABC) and implement `generate(api, version) -> Dict[str, str]`.  
Output rendering is done exclusively through Jinja2 templates; no inline string building in generator code.

## Commands

### Generate code

```bash
# Generate C header + source
python3 glaze_cli.py generate --api gl --version 3.3 --lang c --output-dir out/

# Generate C++ header
python3 glaze_cli.py generate --api gl --version 3.3 --lang cpp --output-dir out/

# Generate both languages in one invocation (--lang is repeatable)
python3 glaze_cli.py generate --api gl --version 4.6 --lang c --lang cpp --output-dir out/

# List all available APIs and versions from the registry
python3 glaze_cli.py list-apis
```

### Build and run tests

```bash
# Build the header-only test
cd tests/test-runtime
cmake -B build && cmake --build build
```

### Data model concepts
- **Feature** — an OpenGL version with `require` and `remove` lists
- **Command** — an OpenGL function with typed parameters
- **Enum** — a named constant grouped by type (generated code uses GL enum symbols, not raw values)
- **Type** — typedef or struct

## Tests

| Test | What it validates |
|------|------------------|
| `tests/test-build/` | Generated C++ header compiles.|
| `tests/test-runtime/` | Full integration: GLFW window, GL 3.3 core profile, dynamic loading, `glGetError()` |
