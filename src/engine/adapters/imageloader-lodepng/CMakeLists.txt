
# lodepng 
add_library("lodepng" 
    "dependencies/lodepng/lodepng.h"
    "dependencies/lodepng/lodepng.cpp"
)

target_include_directories("lodepng" PUBLIC "dependencies/lodepng")

# actual plugin
set (target "XE.Graphics.PNG")
set (sources 
    "include/XE/Graphics/PNG/ImageLoaderPNG.h"

    "src/ImageLoaderPNG.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core lodepng)
