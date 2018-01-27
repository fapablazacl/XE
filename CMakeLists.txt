project (XE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required(VERSION 3.10)

set(XE_VERSION_MAJOR 0)
set(XE_VERSION_MINOR 0)
set(XE_VERSION "${XE_VERSION_MAJOR}.${XE_VERSION_MINOR}")

add_definitions(-DXE_VERSION_MAJOR=${XE_VERSION_MAJOR})
add_definitions(-DXE_VERSION_MINOR=${XE_VERSION_MINOR})

include_directories("src/XE.Core")
add_subdirectory("src/XE.Core")

include_directories("dep/Catch2/single_include")
add_subdirectory("src/XE.Test")

add_subdirectory("src/XE.Graphics.D3D11.TestApp")

add_subdirectory("src/XE.OpenGLCore")
include_directories("src/XE.OpenGLCore")
