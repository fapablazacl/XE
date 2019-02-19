
## What is XE?
XE is a framework, written in C++17 for building high-performance, cross-platform multimedia applications.

## Build Status
| Compiler        | Status        |
| -------------   |:-------------:|
| Visual C++ 2017 | ![status][1]  |

[1]: https://ci.appveyor.com/api/projects/status/github/devwarecl/XE?svg=true

## Preliminary Supported Platforms
* Windows 7+
* Linux (X Window System and Wayland)
* HTML5 
* Android

## Supported Compilers
* Visual C++
* GCC
* clang 

## Feature Status Matrix
| Module            | Feature                                      | Status    |
| -------------     |:-------------:                               | ------    |
| XE.Math           | Linear Algebra                               | Done.     |
| XE.Math           | Collision Detection                          | TODO      |
| XE.Core           | Resource Management                          | Done.     |
| XE.Core (Math)    | Graphics                                     | TODO      |
| XE.Core (Math)    | Audio                                        | TODO      |
| XE.Core (Math)    | Input                                        | TODO      |
| XE.Core (Math)    | Message Bus System                           | Done.     |
| XE.Core (Math)    | Entity Component System                      | TODO      |
| XE.UI             | User Interface                               | TODO      |
| XE.Core (Math)    | Font Rendering                               | Done.     |
| XE.Core (Math)    | Shader Abstraction Layer                     | TODO      |
| XE.Graphics       | Material Graph System                        | TODO      |
| ???               | Asset Content Pipeline                       | TODO      |
| ???               | Asset Runtime Management                     | TODO      |
| ???               | Hardware-accelerated Rat Tracing Rendering   | TODO      |
| ???               | Scene Graph / Scene Graph Renderers          | TODO      |

Features for a Low Level library:
* Math Support Library
    - Linear Algebra (Vector, Matrix and Quaterion operations)
    - Geometry (Triangle, Plane, Box, Sphere and Ellipsoid operations)
