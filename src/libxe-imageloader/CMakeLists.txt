
find_package(FreeImage REQUIRED)

set (target libxe-imageloader)
add_library(${target}
        "include/xe/Image.h"
        "include/xe/ImageLoader.h"
        "src/ImageLoader.cpp"

        # FreeImage
        "src/ImageLoaderFI.h"
        "src/ImageLoaderFI.cpp"
)

target_include_directories(${target} PUBLIC include/)
target_link_libraries(${target} freeimage::FreeImage XE.Core)
