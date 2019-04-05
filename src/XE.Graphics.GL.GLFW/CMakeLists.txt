
# Abtraction utilities around GLFW

set (target XE.Graphics.GL.GLFW)

set (sources
    "XE/Graphics/GL/GLFW/WindowGLFW.hpp"
    "XE/Graphics/GL/GLFW/WindowGLFW.cpp"
    "XE/Graphics/GL/GLFW/GraphicsContextGLFW.hpp"
    "XE/Graphics/GL/GLFW/GraphicsContextGLFW.cpp"
    "XE/Graphics/GL/GLFW/InputManagerGLFW.hpp"
    "XE/Graphics/GL/GLFW/InputManagerGLFW.cpp"
)

add_library(${target} ${sources})

target_link_libraries(${target} glfw XE.Core XE.Graphics.GL)
