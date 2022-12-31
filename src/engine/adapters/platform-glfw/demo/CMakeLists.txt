
set (target "graphics-gl-glfw-demo")

set (sources 
    "Demo.cpp" 
    "Platform.cpp" 
    "Platform.h" 
    "Util.cpp" 
    "Util.h" 
    "Common.cpp" 
    "Common.h" 
    "Ballblazer.h" 
    "Ballblazer.cpp"
)

add_executable(${target} ${sources})

target_include_directories(${target} PUBLIC ".")
target_link_libraries(${target} XE.Core XE.IO XE.Graphics.GL XE.Graphics.GL.GLFW)
