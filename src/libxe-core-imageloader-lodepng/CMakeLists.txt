
find_package(lodepng CONFIG REQUIRED)

set (target "XE.Graphics.PNG")
set (sources 
    "src/xe/graphics/png/ImageLoaderPNG.h"
    "src/xe/graphics/png/ImageLoaderPNG.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target} 
    PUBLIC XE.Core 
    PRIVATE lodepng::lodepng
    PRIVATE XE.Interface
)
