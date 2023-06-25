
find_package(lodepng CONFIG REQUIRED)

set (target "XE.Graphics.PNG")
set (sources 
    "include/xe/graphics/png/ImageLoaderPNG.h"

    "src/ImageLoaderPNG.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} 
    PUBLIC XE.Core 
    PRIVATE lodepng
    PRIVATE XE.Interface
)
