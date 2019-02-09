
set (target "XE.Graphics.PNG")
set (sources 
    "XE/Graphics/PNG/ImageLoaderPNG.hpp"
    "XE/Graphics/PNG/ImageLoaderPNG.cpp"
)

add_library(${target} ${sources})

target_link_libraries(${target} lodepng)
