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
  doc_parser.py          — parses OpenGL-Refpages XML into FunctionDoc entries
  generators/
    base.py              — abstract Generator base class; owns the Jinja2
                           environment and the `_read_static_template` helper
                           used to ship verbatim assets like raii.hpp
    c_generator.py       — renders C header + source via templates/c/
    cpp_generator.py     — renders the C++ binding plus the enriched-handle
                           companion via templates/cpp/
  templates/
    c/gl.h.j2            — Jinja2 template for the C header
    c/gl.c.j2            — Jinja2 template for the C source
    cpp/gl.hpp.j2        — Jinja2 template for the main C++ binding
    cpp/gl_handle.hpp.j2 — Jinja2 template for the enriched-handle companion
                           (gl::handle::* legacy / non-DSA wrappers)
    cpp/static/raii.hpp  — Hand-written, API-agnostic header shipped verbatim
                           (glaze::Unique/Shared/Weak + Traits customization
                           point). NOT a Jinja template — read via
                           `Generator._read_static_template`.
    cmake/CMakeLists.txt.j2 — CMakeLists template emitted at build time so the
                              C library and INTERFACE targets get configured.
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
  test_cpp_generator.py  — C++ generator helper and output tests, including
                           TestCppGeneratorDSA, TestCppGeneratorHandle,
                           TestCppGeneratorRaii (Traits specializations)
  test_string_utils.py   — string utility tests
  test_cli.py            — CLI argument parsing tests
```

Generators inherit from `Generator` (ABC) and implement `generate(api, version) -> dict[str, str]`.
Output rendering goes through Jinja2 templates with one exception: `cpp/static/raii.hpp` is shipped verbatim via `Generator._read_static_template`.

### C++ output layout

A single `python glaze_cli.py generate --api gl X.Y --lang cpp` invocation produces three files in `include/glaze/`:

| File | Contents |
|---|---|
| `{api}.hpp` | The main binding: strong handle types (`gl::BufferId`, `gl::Texture`, …), location types (`gl::UniformLocation`/`gl::AttribLocation`), enum classes, `Flags<E>`, `ArrayView<T>`, per-command functors (`gl::clear`, `gl::genBuffer`, …), and `dsa::*` wrappers for GL 4.5+ DSA functions. Also emits `glaze::Traits<>` specializations at the bottom for raw handle ids and `dsa::*` wrapper classes. Includes `"raii.hpp"`. |
| `{api}_handle.hpp` | Companion library: `gl::handle::*` classes that expose every **legacy (non-`Named`) GL function whose first parameter is a known handle type** as a member method. Method bodies delegate to the corresponding `gl::funcName(...)` functor, so they inherit strong return types (`UniformLocation`, `AttribLocation`, `std::string`, …). Also emits `glaze::Traits<>` specializations for the `handle::*` wrapper classes. |
| `raii.hpp` | The hand-written, API-agnostic smart-pointer header. Defines `glaze::Unique<T>`, `glaze::Shared<T>`, `glaze::Weak<T>`, and the `glaze::Traits<T>` customization point. `glaze::makeUnique<T>()` / `glaze::makeShared<T>()` call `Traits<T>::create()`. Users can manage any T (raw handle id, `dsa::Buffer`, `handle::Program`, …) for which the generated headers provide a `Traits` specialization, **or** specialize `Traits` themselves for their own types. |

### Conan CMake components

| Target | Purpose |
|---|---|
| `glaze::raii` | Top-level header-only component exposing only `raii.hpp`. For consumers that want `glaze::Unique/Shared/Weak` with their own handle types, no per-API binding required. |
| `glaze::{api}` | Per-API binding: links the C loader library and provides `{api}.hpp`.
| `glaze::{api}_handle` | Header-only enriched-handle companion: provides `{api}_handle.hpp`, requires `glaze::{api}`. |

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
- **Object class** — `Command.get_class()` returns the `class_` attribute of the *first* parameter (e.g. `"buffer"`, `"program"`). `Registry.object_dict` indexes commands by that class. Used by both `_build_dsa_classes` (DSA `Named*` wrappers) and `_build_handle_classes` (legacy `handle::*` wrappers, which exclude `Named*`).
- **Smart-pointer Traits** — `glaze::Traits<T>` is the customization point that bridges any value type T into `glaze::Unique/Shared/Weak`. Each specialization must provide `static T create()`, `static void destroy(T&)`, `static bool valid(const T&)`. The C++ generator emits specializations for raw handle ids, `dsa::*` wrapper classes (in `gl.hpp`), and `handle::*` wrapper classes (in `gl_handle.hpp`).

## Tests

Unit tests live in `tests/` and use pytest with a mini-XML fixture (~100 lines) instead of the real 2.5 MB `gl.xml`.

| Test file | What it validates |
|-----------|------------------|
| `test_string_utils.py` | `is_capitalized`, `split_capitalized`, `camel_case` |
| `test_model.py` | Dataclasses (`TypeDecl`, `CommandParam`, `Command`), `Registry` methods (`consolidate`, `collect_features`, `available_apis`) |
| `test_parser.py` | XML parsing of types, enums, commands, features, extensions, and error cases |
| `test_c_generator.py` | C generator file output, enum defines, function typedefs, loader |
| `test_cpp_generator.py` | C++ helpers, main binding output, `TestCppGeneratorDSA` (DSA `Named*` wrappers), `TestCppGeneratorHandle` (legacy `handle::*` wrappers + functor delegation + strong return types), `TestCppGeneratorRaii` (static `raii.hpp` shipped verbatim + `glaze::Traits<>` specializations in `gl.hpp` / `gl_handle.hpp`) |
| `test_cli.py` | Argument parsing for `generate` and `list-apis` subcommands |
