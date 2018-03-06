project (XE)

option (XE_PLUGIN_DIRECT3D11    "Include Direct3D 11 support" OFF)
option (XE_PLUGIN_OPENGLCORE    "Include OpenGL Core support" OFF)
option (XE_PLUGIN_VULKAN        "Include OpenGL Core support" OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required (VERSION 3.10)

set (XE_VERSION_MAJOR 0)
set (XE_VERSION_MINOR 0)
set (XE_VERSION_REVISION 0)
set (XE_VERSION "${XE_VERSION_MAJOR}.${XE_VERSION_MINOR}.${XE_VERSION_REVISION}")

add_definitions (-DXE_VERSION_MAJOR=${XE_VERSION_MAJOR})
add_definitions (-DXE_VERSION_MINOR=${XE_VERSION_MINOR})
add_definitions (-DXE_VERSION_REVISION=${XE_VERSION_REVISION})

include_directories ("src/XE.Core")
add_subdirectory ("src/XE.Core")

include_directories ("dep/Catch2/single_include")
add_subdirectory ("src/XE.Test")

if (XE_PLUGIN_DIRECT3D11)
    add_subdirectory ("src/XE.Graphics.D3D11.TestApp")
    include_directories ("src/XE.Graphics.D3D11.TestApp")
endif ()

if (XE_PLUGIN_OPENGLCORE)
	include_directories ("src/khronos")

    add_subdirectory ("src/glad-gl4")
    include_directories ("src/glad-gl4")

    add_subdirectory ("src/XE.OpenGLCore")
    include_directories ("src/XE.OpenGLCore")
endif ()

if (XE_PLUGIN_VULKAN)
    add_subdirectory ("src/XE.Graphics.Vk.TestApp")
    include_directories ("src/XE.Graphics.Vk.TestApp")
endif ()

add_subdirectory("src/XE.Sandbox")
