
set (target "XE.Graphics.PNG")
set (sources 
    "include/XE/Graphics/PNG/ImageLoaderPNG.hpp"

    "src/ImageLoaderPNG.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.IO XE.Graphics lodepng)
