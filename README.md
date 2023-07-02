
## What is XE?
XE is a framework, written in C++17 for building high-performance and cross-platform multimedia applications.

## Build Status

![Continuous Integration Build Status](https://github.com/fapablazacl/XE/actions/workflows/ci.yml/badge.svg)

## Preliminary Supported Platforms
* Windows 10
* Ubuntu
* macOS

## Supported Compilers
* Visual C++
* GCC
* clang 

## Building
XE uses CMake as it build system generator, and vcpkg for dependency management.
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:\src\vcpkg\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022"
