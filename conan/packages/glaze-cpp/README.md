# glaze-cpp

Standalone C++ port of the Glaze OpenGL binding generator. Functionally equivalent to the Python `conan/packages/glaze/`, but written in C++17 and built by Conan from source. Produces type-safe C++ bindings (`gl::clear`, `gl::Buffer`, `gl::ClearBufferMask::eColorBufferBit`, …) and a companion C loader from the Khronos XML registry.

## Components exported

| Target | Kind | Purpose |
|---|---|---|
| `glaze::raii` | header-only | API-agnostic `glaze::Unique`/`Shared`/`Weak` smart pointers + `Traits` customization point |
| `glaze::{api}` | static or shared lib | Per-API binding. `{api}` is `gl`, `gles2`, `gl_compat`, etc. |
| `glaze::{api}_handle` | header-only | Enriched-handle wrappers for legacy (non-DSA) commands |

The CMake package name is `glaze` — consumers that previously pulled `glaze/1.0.0` (the Python package) can swap to `glaze-cpp/1.0.0` without touching their `find_package()` / `target_link_libraries()` calls.

## Build

```bash
conan create conan/packages/glaze-cpp/ --build=missing \
    -o "glaze-cpp/*:apis=gl:4.6" \
    -o "glaze-cpp/*:language=cpp" \
    -o "glaze-cpp/*:extension_vendors=ARB,KHR,EXT"
```

Add `-o "glaze-cpp/*:with_tests=True"` to build and run the Catch2 unit suite during package creation.

## Options

| Option | Default | Notes |
|---|---|---|
| `apis` | `gl:3.3,gl_compat:4.6,gles1:1.0,gles2:3.2` | Comma-separated `api:version` pairs |
| `language` | `cpp` | One of `c`, `cpp`, `both` |
| `extension_vendors` | (empty) | Comma-separated vendor prefixes (`ARB,KHR,EXT`) |
| `extensions` | (empty) | Comma-separated specific extension names |
| `with_docs` | `True` | Attach Doxygen briefs parsed from OpenGL-Refpages |
| `with_tests` | `False` | Build and run the Catch2 unit suite during package build |
| `shared` | `False` | Build the C loader as a shared library |

## How it differs from `conan/packages/glaze/`

- **Generator language**: C++17 instead of Python 3.10. No `cpython` tool-requires.
- **Runtime footprint during build**: `inja`, `pugixml`, `nlohmann_json`, `cxxopts` (all private, none leak to consumers).
- **Packaged output**: identical shape — same components, same header filenames, same symbols.

## Dev build (no Conan)

The package is a self-contained CMake project, so you can also build it directly:

```bash
cd conan/packages/glaze-cpp
cmake -B build -DGLAZE_CPP_WITH_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

External dependencies (inja, pugixml, nlohmann_json, cxxopts, catch2) need to be resolvable via `find_package` in your dev environment when going this route — Conan handles them automatically when building through the recipe.
