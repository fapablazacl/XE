# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Purpose

glaze is a **code generator** that produces language-specific headers and source files for the OpenGL API from the official Khronos XML registry (`OpenGL-Registry/xml/gl.xml`). It supports GL 1.0–4.6 and GLES 1.0–3.2.

Initially both C and C++11 bindings should be generated.

See @README.md for more information.

## Commands

### Generate code

```bash
# Generate C header + source (include/glaze/gl.h + src/gl.c)
python3 glaze_cli.py generate --api gl --version 1.0 --lang c --output-dir tests/test-gl10

# Generate C++ header (include/glaze/gl.hpp)
python3 glaze_cli.py generate --api gl --version 1.0 --lang cpp --output-dir tests/test-gl10

```
### Build and run tests

```bash
# Build the header-only test (requires GLAD)
cd tests/test-gl10
cmake -B build && cmake --build build
```

### Data model concepts
- **Feature** — an OpenGL version with `require` and `remove` lists
- **Command** — an OpenGL function with typed parameters
- **Enum** — a named constant grouped by type
- **Type** — typedef or struct

## Tests

| Test | What it validates |
|------|------------------|
| `tests/test-gl10/` | Generated C++ header compiles and links with GLAD |
| `tests/test-dynamicLoading/` | Full integration: GLFW window, GL 3.3 core profile, dynamic loading, `glGetError()` |
