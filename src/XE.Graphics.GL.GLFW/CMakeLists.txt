
set (target XE.Graphics.GL.GLFW)

set (sources
    "include/XE/Graphics/GL/GLFW/WindowGLFW.h"
    "include/XE/Graphics/GL/GLFW/GraphicsContextGLFW.h"
    "include/XE/Graphics/GL/GLFW/InputManagerGLFW.h"

    "src/WindowGLFW.cpp"
    "src/GraphicsContextGLFW.cpp"
    "src/InputManagerGLFW.cpp"    
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} glfw XE.Core XE.Input XE.Graphics.GL)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})

# add_subdirectory(demo)
