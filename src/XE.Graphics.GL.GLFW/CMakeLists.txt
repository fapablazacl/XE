
set (target XE.Graphics.GL.GLFW)

set (headers
    "include/XE/Graphics/GL/GLFW/WindowGLFW.h"
    "include/XE/Graphics/GL/GLFW/GraphicsContextGLFW.h"
    "include/XE/Graphics/GL/GLFW/InputManagerGLFW.h"
)

set (sources
    ${headers}
    "src/WindowGLFW.cpp"
    "src/GraphicsContextGLFW.cpp"
    "src/InputManagerGLFW.cpp"    
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} glfw XE.Core XE.Input XE.Graphics)

# add_subdirectory(demo)
