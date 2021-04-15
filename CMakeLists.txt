project (XE)

option (XE_PLUGIN_PNG           "Include PNG image support via LodePNG" OFF)
option (XE_PLUGIN_DIRECT3D11    "Include Direct3D 11 support" OFF)
option (XE_PLUGIN_GL            "Include OpenGL 3+ support" OFF)
option (XE_PLUGIN_ES2           "Include OpenGL ES 2 support" OFF)
option (XE_PLUGIN_GL_GLFW       "Include GLFW-based Window and Context creation support (requires 'XE_PLUGIN_GL')" OFF)
option (XE_PLUGIN_VULKAN        "Include Vulkan support" OFF)

option (XE_EXAMPLE_QT5RENDERING "Include example program that shows rendering to a Qt5 widget" OFF)
option (XE_EXAMPLE_DEMO         "Include example program that shows a tech demo" OFF)
option (XE_EXAMPLE_SANDBOX      "Include example program that is used for various development activities" OFF)
option (XE_TEST "Include unit-test suite" OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required (VERSION 3.10)

set (XE_VERSION_MAJOR 0)
set (XE_VERSION_MINOR 0)
set (XE_VERSION_REVISION 0)
set (XE_VERSION "${XE_VERSION_MAJOR}.${XE_VERSION_MINOR}.${XE_VERSION_REVISION}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions (-D_DEBUG)
endif ()

add_definitions (-DXE_VERSION_MAJOR=${XE_VERSION_MAJOR})
add_definitions (-DXE_VERSION_MINOR=${XE_VERSION_MINOR})
add_definitions (-DXE_VERSION_REVISION=${XE_VERSION_REVISION})

include_directories ("src/XE.Core")
add_subdirectory ("src/XE.Core")

add_subdirectory ("src/XE.Graphics")
add_subdirectory ("src/XE.Scene")
add_subdirectory ("src/XE.Input")
add_subdirectory ("src/XE.Math")

#========================================================
# PLUGINS SECTION
#========================================================

# include LodePNG image
if (XE_PLUGIN_PNG)
    include_directories("dep/lodepng")
    set (lodepng_target "lodepng")
    set (lodepng_sources 
        "dep/lodepng/lodepng.h"
        "dep/lodepng/lodepng.cpp"
        # "dep/lodepng/lodepng_util.h"
        # "dep/lodepng/lodepng_util.cpp"
    )

    add_library(${lodepng_target} ${lodepng_sources})

    include_directories("src/XE.Graphics.PNG")
    add_subdirectory("src/XE.Graphics.PNG")
endif ()

if (XE_TEST)
    include_directories ("dep/Catch2/single_include")
    add_subdirectory ("src/XE.Test")
endif()

if (XE_PLUGIN_DIRECT3D11)
    add_subdirectory ("src/XE.Graphics.D3D11.TestApp")
    include_directories ("src/XE.Graphics.D3D11.TestApp")
endif ()

if (XE_PLUGIN_GL)
    include_directories ("src/khronos")

    add_subdirectory ("src/glad-gl4")
    include_directories ("src/glad-gl4")

	set (GLFW_BUILD_DOCS CACHE BOOL OFF)
	set (GLFW_BUILD_TESTS CACHE BOOL OFF)
	set (GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
	set (GLFW_BUILD_INSTALL CACHE BOOL OFF)
	set (GLFW_INSTALL CACHE BOOL OFF)
	add_subdirectory ("dep/glfw")
	include_directories ("dep/glfw/include")

    add_subdirectory ("src/XE.Graphics.GL")
    include_directories ("src/XE.Graphics.GL")

    if (XE_PLUGIN_GL_GLFW)
        add_subdirectory("src/XE.Graphics.GL.GLFW")
        include_directories("src/XE.Graphics.GL.GLFW")
    endif()
endif ()


if (XE_PLUGIN_ES2)
    include_directories ("src/khronos")

    add_subdirectory ("src/glad-es2")
    include_directories ("src/glad-es2")

	set (GLFW_BUILD_DOCS CACHE BOOL OFF)
	set (GLFW_BUILD_TESTS CACHE BOOL OFF)
	set (GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
	set (GLFW_BUILD_INSTALL CACHE BOOL OFF)
    set (GLFW_INSTALL CACHE BOOL OFF)
    
    add_subdirectory ("src/XE.Graphics.ES2")
    include_directories ("src/XE.Graphics.ES2")
endif()

if (XE_PLUGIN_VULKAN)
	set (GLFW_BUILD_DOCS CACHE BOOL OFF)
	set (GLFW_BUILD_TESTS CACHE BOOL OFF)
	set (GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
	set (GLFW_BUILD_INSTALL CACHE BOOL OFF)
	set (GLFW_INSTALL CACHE BOOL OFF)
	add_subdirectory ("dep/glfw")
	include_directories ("dep/glfw/include")

    add_subdirectory ("src/XE.Graphics.Vk.TestApp")
    include_directories ("src/XE.Graphics.Vk.TestApp")
endif ()


#========================================================
# EXAMPLES SECTION
#========================================================
if (XE_EXAMPLE_DEMO)
    add_subdirectory("examples/XE.Demo")
endif()

if (XE_EXAMPLE_SANDBOX)
    add_subdirectory("examples/XE.Sandbox")
endif()

if (XE_EXAMPLE_QT5RENDERING)
    add_subdirectory("examples/Qt5Rendering")
endif()
