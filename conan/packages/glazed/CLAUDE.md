# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with
code in this repository.

## Project purpose

`glazed` is a Conan package that **wraps** [glaze](../glaze/) and ships
prebuilt OpenGL bindings. It exists to remove Python from the consumer build
chain: the generator runs once on the publisher's machine, the output is
packaged, and consumers pull a binary-only Conan package.

See @README.md for the consumer-facing usage docs.

## Architecture

```
conan/packages/glazed/
├── conanfile.py                       — Recipe; depends on glaze + cpython as
│                                        tool_requires; runs glaze_cli.py once
│                                        per API family at the highest version
├── templates/
│   └── cmake/
│       └── glazed-targets.cmake.j2    — Jinja template emitting an INTERFACE
│                                        target per API family. Shipped via
│                                        cpp_info.cmake_build_modules so
│                                        find_package(glazed) creates the
│                                        glazed::* targets automatically.
├── test_package/                       — Conan 2 test_package: includes
│                                        gl.hpp, links glazed::gl, runs.
└── (build-time, .gitignored)
    ├── OpenGL-Registry/xml/gl.xml      — Sparse-cloned by source()
    ├── OpenGL-Refpages/                — Sparse-cloned by source()
    └── build/generated/
        ├── include/glaze/{api}.{h,hpp}
        ├── include/glaze/{api}_handle.hpp
        ├── include/glaze/raii.hpp
        └── src/{api}.c
```

**Generated content under `include/` and `src/` is not committed.** It is
populated by `conan create .` from a clean clone. Committing only the recipe
+ templates avoids drift between the generator and its output.

## Generation matrix

The matrix is intentionally tiny — 5 entries — because glaze emits a single
`{api}.hpp` per API family regardless of version, with compile-time feature
gates keyed off `GLAZE_*_VERSION` macros. Generating one binding at the
highest version covers every lower version too.

```python
GLAZED_MATRIX = [
    ("gl",        "4.6"),
    ("gl_compat", "4.6"),
    ("gles1",     "1.0"),
    ("gles2",     "3.2"),
    ("glsc2",     "2.0"),
]
```

To add a new API family, append to `GLAZED_MATRIX` in `conanfile.py` and add
its name to the loop in `templates/cmake/glazed-targets.cmake.j2`.

## How glazed invokes glaze

1. `build_requirements()` declares `tool_requires("glaze/1.0.0")` and
   `tool_requires("cpython/3.10.14")`. Tool-requires are not transitive in
   Conan 2, so glazed must pull cpython itself.
2. `build()` reads two conf values exposed by glaze's `package_info()`:
   - `user.glaze:source_dir` → `<glaze pkg>/share/glaze/`
   - `user.glaze:cli_path`   → `<glaze pkg>/share/glaze/glaze_cli.py`
3. `_prepare_python()` pip-installs the shipped glaze tree into a local
   `site-packages` so `glaze_cli.py`'s `from glaze.parser import …` resolves.
4. A single `glaze_cli.py generate` invocation walks the matrix with
   repeated `--api` arguments and writes everything into
   `build/generated/{include,src}/`.

## Build module strategy

Conan components for INTERFACE-with-sources targets are awkward, so glazed
sidesteps them entirely:

- `cpp_info` declares **no components**.
- `cpp_info.set_property("cmake_build_modules", ["share/glazed/glazed-targets.cmake"])`
  ships a single CMake module that creates every `glazed::*` target as a
  non-imported `INTERFACE` library with the loader `.c` attached via
  `target_sources INTERFACE`.

This pattern requires CMake 3.13+ (which is well below glaze's own 3.15
minimum).

## Required additive coupling to glaze

`conan/packages/glaze/conanfile.py` carries a small additive patch so its
`package()` copies `glaze_cli.py`, `pyproject.toml`, and the entire `glaze/`
Python tree (`*.py`, `*.j2`, `*.hpp`) into `${package_folder}/share/glaze/`,
and its `package_info()` exposes `user.glaze:source_dir` and
`user.glaze:cli_path` via `conf_info`. Existing glaze consumers are
unaffected — they never look at `share/glaze/`.

## Verification

```bash
# From repo root
conan create conan/packages/glaze   --build=missing
conan create conan/packages/glazed  --build=missing
```

Expected:
- glaze's package now contains `share/glaze/glaze_cli.py`, the `glaze/`
  tree, and `pyproject.toml` alongside its existing `include/`/`lib/`.
- glazed's `build()` calls `glaze_cli.py generate` exactly once, writes
  `include/glaze/{gl,gl_compat,gles1,gles2,glsc2}.{h,hpp}` plus the
  `_handle.hpp` companions, `raii.hpp`, and `src/{api}.c` files.
- `test_package` builds `test_glazed_gl.cpp` against `glazed::gl`, the
  consumer-side compile of `src/gl.c` succeeds, and the binary exits 0.

A consumer-side smoke test (no Python on PATH) would be:
```bash
conan install --requires=glazed/1.0.0 --deployer=full_deploy
```
This should resolve and place headers + sources without ever fetching
cpython, since glazed's tool_requires are skipped on cache hit.

## Out of scope for the initial scaffold

- CI workflow to publish glazed to a remote registry
- Per-version test matrix (currently only one `GLAZE_GL_VERSION` is exercised)
- Refpages caching to speed up rebuilds
- Resolving any per-API generator failures discovered when running the full
  matrix end-to-end for the first time
