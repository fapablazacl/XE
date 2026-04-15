find_package(pugixml CONFIG REQUIRED)

set (target "glaze-docparser")

set (sources
    "src/xe/glaze/docparser/FunctionDoc.h"
    "src/xe/glaze/docparser/DocIndex.h"
    "src/xe/glaze/docparser/DocParser.h"
    "src/xe/glaze/docparser/DocParser.cpp"
)

add_library(${target} ${sources})
add_library(xe::glaze-docparser ALIAS ${target})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target}
    PRIVATE xe::interface
    PRIVATE pugixml::pugixml
)
