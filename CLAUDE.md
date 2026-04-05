# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XE is a barebones multiplatform C++17 game engine with OpenGL rendering, GLTF model loading, math/scene libraries, and demo applications (Capybaria game, Apostate game and GLTF viewer, among other tools).

## Build Commands

### Initial Setup (Conan + CMake)
```
make configure
```
This runs `conan install` for Release and Debug, then `cmake --preset conan-release` and `cmake --preset conan-debug`.

### Build
```
cmake --build build/Debug
cmake --build build/Release
```

### Run Tests
```
make test
# or directly:
ctest --test-dir build/Debug --output-on-failure
```
Tests require `XE_DEV_UNIT_TEST=ON` in CMake configuration.

### Code Quality
```
make format      # clang-format all src/ files
make tidy        # clang-tidy static analysis
make tidy-fix    # auto-fix clang-tidy issues
make cppcheck    # cppcheck analysis
make iwyu        # include-what-you-use
```

### Docker Builds
All make targets have `docker-` prefixed equivalents (e.g., `make docker-configure`, `make docker-test`). Build the Docker image first with `make docker`.

## Architecture

### Core Libraries (in `src/`)
- **libxe-core** — Engine foundation: graphics device abstractions, textures, shaders, programs, materials, window management, input, IO streams
- **libxe-gl** — OpenGL implementations of core abstractions (GraphicsDeviceGL, textures, shaders, renderers)
- **libxe-math** — Vectors, matrices, quaternions, geometric primitives (Box, Plane, Ray, Sphere, Triangle)
- **libxe-scene** — Scene management: trackball camera, virtual sphere, projection
- **libxe-geometry** — Geometry utilities
- **libxe-imageloader** / **libxe-core-imageloader-lodepng** — Image loading interfaces and PNG implementation
- **libxe-core-platform-glfw** — GLFW-based window/context/input
- **libxe-app** — Application base class and platform abstraction

### Applications
- **xe-gltf-view** — GLTF model viewer (ImGui UI, SDL2 windowing, OpenGL rendering)
- **capybaria** — Multiplayer capybara battle game
- **apostate** — Demo project

### Tools
- **xe-gltfc** — GLTF 3d model compiler tool
- **xe-ktxc** — KTX texture compiler tool

### Dependency Chain
```
XE.Core → XE.Math, fmt, glfw
XE.GL → XE.Core, glad, glm, glfw
XE.App → XE.Core, XE.GL
xe-gltf-view → XE.Core, XE.GL, XE.App, cgltf, imgui, SDL2
```

### Package: Glaze (`conan/packages/glaze/`)
A Conan-packaged OpenGL binding code generator. Parses Khronos XML registry and generates C/C++ headers via Jinja2 templates. Has its own CLAUDE.md, pytest suite, and CI workflow.

## Key CMake Options
- `XE_PLUGIN_PNG`, `XE_PLUGIN_GL`, `XE_PLUGIN_GL_GLFW` — Feature toggles (all ON by default)
- `XE_DEV_UNIT_TEST` — Enable tests (OFF by default)
- `XE_DEV_WARNINGS_AS_ERRORS`, `XE_DEV_INSTRUMENT_COVERAGE`, `XE_DEV_SANITIZER_ENABLE` — Dev options

## Code Style
- LLVM-based clang-format: 4-space indent, no tabs, 180 char column limit, attach braces, pointer right-aligned (`int *p`)
- clang-tidy checks: clang-analyzer, bugprone, concurrency, performance, cppcoreguidelines

## Dependencies (via Conan)
imgui, assimp, glfw, cgltf, fmt, lodepng, ms-gsl, devil, glm, nlohmann_json, vulkan-loader, ktx, cxxopts, glaze, sdl2
