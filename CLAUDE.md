# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XE is a barebones multiplatform C++17 game engine with OpenGL rendering, GLTF model loading, math/scene libraries, and demo applications (Capybaria game, Apostate game and GLTF viewer, among other tools). It uses OpenGL 3.3 as the primary rendering backend (with an OpenGL 4.6 backend available). The current active branch (`feature/gltf-view`) focuses on a glTF 2.0 model viewer.

## Code Conventions
- instance types (AKA polymorphic type) should be classes with PascalCase naming.
- value types must be represented as structs, and global functions and methods must be camelCase.
- each new test should have their own unit test suite.
- public classes and methods must have extended Doxygen documentation
- member variables must have a single line Doxygen comment above them.
- Some examples of these conventions:
```C++
/**
 * @brief what it this (classes / structs)
 * Extended description on why it is neccesary | how it does it, plus secondary effects
 * @param param1 description
 * @param param2 description
 * @return description
 */
class Foo {
public:
    /**
    * @brief what it does
    * how it does, indicating which member variables mutate.
    * @param param1 description
    * @param param2 description
    * @return description
    */
    int computeValue(int a, int b) const;

    virtual ~Foo() = default;

private:
    //! Just a single comment suffices here
    int intervalMember = 0;
};
```

## Build System

**Prerequisites:** CMake 3.16+, Conan 2.x

### CMake Conventions
- Each target must be live inside in the "src" folder, into its own folder there.
- library related targets start with the "lib" prefix.
- each library target must a companion test target, with the same base as the library target adding the "-test" suffix.
- executable related targets start with the "xe-" prefix.
- source code must live in a "src" sub folder.
- Example CMakeLists.txt form:

```bash
find_package(package1 REQUIRED)
find_package(package2 REQUIRED)

set (target libxe-core)
set (sources "src/main.cpp")

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "src")

target_link_libraries(${target} PRIVATE package1::bar)
target_link_libraries(${target} PRIVATE package2::foo)
```

### Configure and build

```bash
# Install deps + configure both Release and Debug
make configure

# Or manually (ensure you use the proper profile from conan/profiles/):
conan install . --build=missing --profile=conan/profiles/<your-profile> --settings=build_type=Release
conan install . --build=missing --profile=conan/profiles/<your-profile> --settings=build_type=Debug
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
| `XE_DEV_UNIT_TEST` | ON | Enable GTest unit tests |
| `XE_DEV_WARNINGS_AS_ERRORS` | OFF | Treat warnings as errors |
| `XE_DEV_PRECOMPILED_HEADERS` | OFF | PCH for faster builds |
| `XE_DEV_SANITIZER_ENABLE` | OFF | AddressSanitizer |
| `XE_DEV_INSTRUMENT_COVERAGE` | OFF | Instrument source files for code coverage |

### Running tests

Tests require `-DXE_DEV_UNIT_TEST=ON` at configure time.

```bash
make test
# or directly:
ctest --test-dir build/Debug --output-on-failure
```

## Architecture

### Libraries (in `src/`)

All libraries live under `src/` and are prefixed `libxe-`:

- **libxe-core** — Engine foundation: abstract `GraphicsDevice` interface, shader/program/texture/buffer/material abstractions, image loading, I/O streams, input, logging, timing, window management.
- **libxe-math** — Math primitives: vectors, matrices, quaternions, boundary types (Box, Plane, Ray, Rect, Sphere, Triangle, Ellipsoid), VectorExpr for SIMD.
- **libxe-gl** — OpenGL abstractions (GraphicsDeviceGL, RendererGL, textures, shaders, programs, buffers). Uses GLAD for function loading.
- **libxe-scene** — Scene management/camera control: `Projection`, `Trackball`, `VirtualSphere`.
- **libxe-geometry** — Procedural shape generation/geometry utilities: boxes, planes, ellipsoids.
- **libxe-app** — Application base class and platform abstraction layer (`Platform.h`).
- **libxe-core-platform-glfw** — GLFW implementation: window, GL context, input.
- **libxe-imageloader** / **libxe-core-imageloader-lodepng** — Image loading interfaces (DevIL-based) and LodePNG PNG loader.

### Applications

- **xe-gltf-view** — Interactive glTF 2.0 viewer. Uses cgltf, SDL2, ImGui, OpenGL rendering. Entry: `xe-gltf-view <path.gltf>`. Key classes: `GltfDataLoader`, `GltfRenderer`, `Window`.
- **apostate** — Proof-of-concept game demo, 1st person shooter.
- **capybaria** — Multiplayer capybara battle game / 3rd person adventure game.
- **vulkan-app-poc / d3d11-app-poc** — Experimental graphics API prototypes.

### Tools

- **xe-gltfc** — glTF 3d model compiler/converter (Assimp, KTX, DevIL).
- **xe-ktxc** — KTX texture compiler/converter (Vulkan SDK, cxxopts).
- **Package: Glaze (`conan/packages/glaze/` / `tools/OpenGL-Hpp`)** — A Conan-packaged Python code generator producing OpenGL C/C++ bindings from the Khronos XML registry (GL 1.0–4.6) via Jinja2 templates. Has its own `CLAUDE.md`, pytest suite, and CI workflow.

## Code Style

- C++17, no extensions (`set(CMAKE_CXX_EXTENSIONS OFF)`)
- clang-format with LLVM base style (see `.clang-format`): 4-space indent, no tabs, 180 char column limit, attach braces, pointer right-aligned (`int *p`)
- clang-tidy with full checks: clang-analyzer, bugprone, concurrency, performance, cppcoreguidelines (see `.clang-tidy`)
- ImGui backend bindings in `src/xe-gltf-view/src/bindings/` are excluded from tidy/cppcheck

## Dependencies (via Conan)

imgui, assimp, glfw, cgltf, fmt, lodepng, ms-gsl, devil, glm, nlohmann_json, vulkan-loader, ktx, cxxopts, glaze, sdl2

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
