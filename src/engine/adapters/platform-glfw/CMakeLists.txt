
add_subdirectory(dependencies/glfw)

find_package(glfw3 REQUIRED)

set (target XE.Graphics.GL.GLFW)

set (headers
    "include/xe/graphics/gl/GLFW/WindowGLFW.h"
    "include/xe/graphics/gl/GLFW/GraphicsContextGLFW.h"
    "include/xe/graphics/gl/GLFW/InputManagerGLFW.h"
)

set (sources
    ${headers}
    "src/WindowGLFW.cpp"
    "src/GraphicsContextGLFW.cpp"
    "src/InputManagerGLFW.cpp"    
)

add_library(${target} ${sources})

target_precompile_headers(${target} PRIVATE ${headers})
target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} glfw XE.Core XE.Input XE.Graphics)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${target} PRIVATE -Wall -Wextra)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Werror)
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${target} PRIVATE /Wall)

    if (XE_WARNINGS_AS_ERRORS)
        target_compile_options(${target} PRIVATE /WX)
    endif()
endif()
