# glazed

`glazed` is a Conan package containing **prebuilt** OpenGL C and C++ bindings
for every API family supported by [glaze](../glaze/). It exists so that
consumers can pull a single Conan package, link a CMake target, and start
using OpenGL without ever invoking Python or processing `gl.xml`.

## How it differs from glaze

| | glaze | glazed |
|---|---|---|
| Generator runs at | Consumer build time | Publisher build time |
| Consumer needs Python | Yes | **No** |
| API matrix | Selected via `apis` option | Always full (gl, gl_compat, gles1, gles2, glsc2) |
| Loader compilation | Built into static lib at glaze build time | `.c` source compiled by consumer |
| Output | One `.hpp` per requested (api, version) | One `.hpp` per API family at highest version |

glaze is the right choice when a consumer wants a tightly-trimmed, fully
prebuilt static library and is willing to install Python at build time.
glazed is the right choice when consumers want zero Python in their build
chain and would rather link prebuilt headers, configuring the GL version
through a single compile-time macro.

## Usage

```python
# consumer conanfile.py
def requirements(self):
    self.requires("glazed/1.0.0")
```

```cmake
# consumer CMakeLists.txt
find_package(glazed REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE glazed::gl)
target_compile_definitions(my_app PRIVATE GLAZE_GL_VERSION=46)
```

```cpp
// consumer main.cpp
#include <glaze/gl.hpp>

int main() {
    gl::clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gl::clear(gl::ClearBufferMask::eColorBufferBit);
    return 0;
}
```

## Available CMake targets

- `glazed::raii` — header-only `glaze::Unique` / `Shared` / `Weak`
- `glazed::gl` — OpenGL core profile bindings (1.0–4.6)
- `glazed::gl_compat` — OpenGL compatibility profile bindings (1.0–4.6)
- `glazed::gles1` — OpenGL ES 1.0 bindings
- `glazed::gles2` — OpenGL ES 2.0–3.2 bindings
- `glazed::glsc2` — OpenGL SC 2.0 bindings
- `glazed::<api>_handle` — enriched-handle companions for each API above

Each non-handle target carries the per-API loader `.c` source via
`target_sources INTERFACE`, so the consumer's compiler builds exactly the
loader that was linked.

## Selecting a GL version

`glazed::gl` ships the full 1.0–4.6 binding gated by `GLAZE_GL_VERSION`.
Pick the version you need with a compile definition:

```cmake
target_compile_definitions(my_app PRIVATE GLAZE_GL_VERSION=33)  # GL 3.3
target_compile_definitions(my_app PRIVATE GLAZE_GL_VERSION=46)  # GL 4.6
```

The same applies to `GLAZE_GLES_VERSION`, `GLAZE_GL_COMPAT_VERSION`, etc.
