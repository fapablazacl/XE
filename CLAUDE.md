# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XE is a barebones multiplatform game engine in modern C++17. It uses OpenGL 3.3 as the primary rendering backend. The current active branch (`feature/gltf-view`) focuses on a glTF 2.0 model viewer.

## Build System

**Prerequisites:** CMake 3.16+, Conan 2.x

### Configure and build

```bash
# Install deps + configure both Release and Debug
make configure

# Or manually:
conan install . --build=missing --settings=build_type=Release
conan install . --build=missing --settings=build_type=Debug
cmake --preset conan-release
cmake --preset conan-debug

# Build
cmake --build --preset conan-release
cmake --build --preset conan-debug

# Build (Visual C++)
cmake --build --preset conan-release
cmake --build --preset conan-debug
```

Conan generates CMake presets in `build/generators/CMakePresets.json`. Always run `conan install` before `cmake --preset`.

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `XE_PLUGIN_PNG` | ON | PNG support via LodePNG |
| `XE_PLUGIN_GL` | ON | OpenGL 3+ backend |
| `XE_PLUGIN_GL_GLFW` | ON | GLFW window/context |
| `XE_DEV_UNIT_TEST` | OFF | Enable GTest unit tests |
| `XE_DEV_WARNINGS_AS_ERRORS` | OFF | Treat warnings as errors |
| `XE_DEV_PRECOMPILED_HEADERS` | OFF | PCH for faster builds |
| `XE_DEV_SANITIZER_ENABLE` | OFF | AddressSanitizer |

### Code quality

```bash
make format       # clang-format all sources in src/
make tidy         # run-clang-tidy against build/Debug/
make tidy-fix     # auto-fix clang-tidy issues
make cppcheck     # exhaustive cppcheck (excludes imgui bindings)
make iwyu         # include-what-you-use check
make iwyu-fix     # auto-fix includes
```

### Running tests

Tests require `-DXE_DEV_UNIT_TEST=ON` at configure time.

```bash
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

### Docker workflow

```bash
make docker             # Build Docker image (Ubuntu)
make docker-configure   # Configure inside container
make docker-format      # clang-format inside container
make docker-tidy        # clang-tidy inside container
make docker-test        # Run tests inside container
```

## Architecture

### Library structure

All libraries live under `src/` and are prefixed `libxe-`:

- **libxe-core** — Engine foundation: abstract `GraphicsDevice` interface, shader/program/texture/buffer abstractions, image loading, I/O streams, input, logging, timing.
- **libxe-math** — Math primitives: vectors, matrices, quaternions, boundary types (Box, Plane, Ray, Rect, Sphere, Triangle, Ellipsoid), VectorExpr for SIMD.
- **libxe-gl** — OpenGL 4.6 backend implementing `GraphicsDevice`. Uses GLAD for function loading. Key classes: `GraphicsDeviceGL`, `RendererGL`, GL texture/shader/buffer wrappers, `TextureRepository`.
- **libxe-scene** — Camera control: `Projection`, `Trackball`, `VirtualSphere`.
- **libxe-geometry** — Procedural shape generation: boxes, planes, ellipsoids.
- **libxe-app** — Platform abstraction layer (`Platform.h`).
- **libxe-core-platform-glfw** — GLFW implementation: window, GL context, input.
- **libxe-imageloader** — DevIL-based image loading.
- **libxe-core-imageloader-lodepng** — LodePNG PNG loader.

### Executables

- **xe-gltf-view** — Interactive glTF 2.0 viewer. Uses cgltf, SDL2, ImGui. Entry: `xe-gltf-view <path.gltf>`. Key classes: `GltfDataLoader`, `GltfRenderer`, `Window`.
- **xe-gltfc** — glTF compiler/converter (Assimp, KTX, DevIL).
- **xe-ktxc** — KTX texture converter (Vulkan SDK, cxxopts).
- **apostate** — Proof-of-concept game demo, 1st person shooter
- **capybaria** — Proof-of-concept game demo, 3rd person adventure game
- **vulkan-app-poc / d3d11-app-poc** — Experimental graphics API prototypes.

### Tools

- **tools/OpenGL-Hpp/** — Python code generator producing OpenGL C/C++ bindings from the Khronos XML registry (GL 1.0–4.6). Has its own `CLAUDE.md`.

## Code Style

- C++17, no extensions (`set(CMAKE_CXX_EXTENSIONS OFF)`)
- clang-format with LLVM base style (see `.clang-format`)
- clang-tidy with full bugprone/performance/cppcoreguidelines checks (see `.clang-tidy`)
- ImGui backend bindings in `src/xe-gltf-view/src/bindings/` are excluded from tidy/cppcheck

## Conan Profiles

All profiles are in `conan/profiles/`. Use `--profile=conan/profiles/<name>` with `conan install`.

| Profile | OS | Arch | Compiler |
|---|---|---|---|
| `x64-win-gcc` | Windows | x86_64 | GCC 5.1 |
| `x86-win-gcc` | Windows | x86 | GCC 5.1 |
| `x64-win-msvc` | Windows | x86_64 | MSVC 194 (VS 2022) |
| `arm64-mac-clang` | macOS | arm64 | Apple Clang 15 |
| `x64-mac-clang` | macOS | x86_64 | Apple Clang 15 |
| `armv6hf-linux-gcc` | Linux | armv6 | GCC 13 |
| `arm-linux-gcc` | Linux | armv7hf | GCC 13 |
| `arm64-linux-gcc` | Linux | armv8 | GCC 13 |
| `x86-linux-gcc` | Linux | x86 | GCC 13 |
| `x64-linux-gcc` | Linux | x86_64 | GCC 13 |

**macOS universal binaries:** Conan has no `arch=universal`. Build both macOS profiles separately and merge with `lipo`:

```bash
conan install . --build=missing --profile=conan/profiles/arm64-mac-clang
conan install . --build=missing --profile=conan/profiles/x64-mac-clang
lipo -create -output libxe.dylib arm64/libxe.dylib x64/libxe.dylib
```

Adjust `compiler.version` in the macOS profiles to match the installed Xcode version.

## Notes

- imgui backends (glfw, opengl3, sdl2, sdl3) are auto-copied from the Conan package into `src/xe-gltf-view/src/bindings/` during `conan install`.
- VulkanSDK must be downloaded manually; the Conan package provides only the loader.
- `compile_commands.json` is generated at `build/Debug/compile_commands.json` and copied to the root for IDE/tooling use.
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
