
# Abtraction utilities around GLFW

set (target XE.Graphics.GL.GLFW)

set (sources
    "include/XE/Graphics/GL/GLFW/WindowGLFW.hpp"
    "include/XE/Graphics/GL/GLFW/GraphicsContextGLFW.hpp"
    "include/XE/Graphics/GL/GLFW/InputManagerGLFW.hpp"

    "src/WindowGLFW.cpp"
    "src/GraphicsContextGLFW.cpp"
    "src/InputManagerGLFW.cpp"    
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} glfw XE.Core XE.Input XE.Graphics.GL)
