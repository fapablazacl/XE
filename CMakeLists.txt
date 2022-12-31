
cmake_minimum_required (VERSION 3.16)

project (XE)

option (XE_PLUGIN_PNG           "Include PNG image support via LodePNG" OFF)
option (XE_PLUGIN_DIRECT3D11    "Include Direct3D 11 support" OFF)
option (XE_PLUGIN_GL            "Include OpenGL 3+ support" OFF)
option (XE_PLUGIN_ES2           "Include OpenGL ES 2 support" OFF)
option (XE_PLUGIN_GL_GLFW       "Include GLFW-based Window and Context creation support (requires 'XE_PLUGIN_GL')" OFF)
option (XE_PLUGIN_VULKAN        "Include Vulkan support" OFF)

option (XE_EXAMPLE_DEMO         "Include example program that shows a tech demo" OFF)
option (XE_EXAMPLE_SANDBOX      "Include example program that is used for various development activities" OFF)
option (XE_UNIT_TEST            "Include unit-test suite" ON)
option (XE_WARNINGS_AS_ERRORS   "" ON)

set (CMAKE_CXX_STANDARD             17)
set (CMAKE_CXX_STANDARD_REQUIRED    ON)
set (CMAKE_CXX_EXTENSIONS           OFF)

# TODO: Configure the version numbering mechanism
set (XE_VERSION_MAJOR 1)
set (XE_VERSION_MINOR 0)
set (XE_VERSION_REVISION 0)
set (XE_VERSION "${XE_VERSION_MAJOR}.${XE_VERSION_MINOR}.${XE_VERSION_REVISION}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    # this macro is required by Glad.
    add_definitions (-D_DEBUG)
endif ()

add_definitions (-DXE_VERSION_MAJOR=${XE_VERSION_MAJOR})
add_definitions (-DXE_VERSION_MINOR=${XE_VERSION_MINOR})
add_definitions (-DXE_VERSION_REVISION=${XE_VERSION_REVISION})

add_subdirectory ("src")
