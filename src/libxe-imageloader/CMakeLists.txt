
find_package(DevIL)

set (target libxe-imageloader)
add_library(${target}
        "include/xe/Image.h"
        "include/xe/ImageLoader.h"
        "src/ImageLoader.cpp"

        "src/ImageLoaderIL.h"
        "src/ImageLoaderIL.cpp"
)

target_include_directories(${target} PUBLIC include/)
target_link_libraries(${target} DevIL::DevIL XE.Core)
