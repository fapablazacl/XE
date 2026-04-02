# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Purpose

glaze is a **code generator** that produces language-specific headers and source files for the OpenGL API from the official Khronos XML registry (`OpenGL-Registry/xml/gl.xml`). It supports GL 1.0–4.6 and GLES 1.0–3.2.

Initially both C and C++11 bindings should be generated.

See @README.md for more information.

## Commands

### Generate code

```bash
# Generate C++ header (gl33.hpp) — legacy path
python3 main.py

# Generate C header + source (include/oglhpp/gl.h + src/gl.c) — modern path
python3 main2.py

# Generate GLAD loader bindings
./generate-glad.sh
```

### Build and run tests

```bash
# Build the header-only test (requires GLAD)
cd tests/test-gl10
cmake -B build && cmake --build build

# Build the dynamic loading test (requires GLFW3)
cd tests/test-dynamicLoading
cmake -B build && cmake --build build
```

## Legacy Architecture

The project has two parallel generation pipelines:

### C++ pipeline (`main.py`)
```
OpenGL-Registry/xml/gl.xml
  → oglhpp/glregistry.py  (GLXMLParser → Repository data model)
  → oglhpp/hppgenerator.py (CodeGenerator)
  → tests/test-gl10/gl33.hpp
```
Produces a single `.hpp` with inline functions, `enum class`, strong typedefs, and `std::span` support. All symbols live in the `gl` namespace.

### C pipeline (`main2.py`)
```
OpenGL-Registry/xml/gl.xml
  → oglhppgen/model.py  (RegistryFactory → Registry data model)
  → oglhppgen/c_generator.py (C_Generator)
  → include/oglhpp/gl.h + src/gl.c
```
Produces a header declaring function pointers and a source file implementing `oglhpp_load_functions()` for dynamic loading.

### Shared utilities
- `oglhpp/string_utils.py` — name-conversion helpers (`split_capitalized`, `camel_case`, etc.)

### Data model concepts (common to both paths)
- **Feature** — an OpenGL version with `require` and `remove` lists
- **Command** — an OpenGL function with typed parameters
- **Enum** — a named constant grouped by type
- **Type** — typedef or struct

## Tests

| Test | What it validates |
|------|------------------|
| `tests/test-gl10/` | Generated C++ header compiles and links with GLAD |
| `tests/test-dynamicLoading/` | Full integration: GLFW window, GL 3.3 core profile, dynamic loading, `glGetError()` |

