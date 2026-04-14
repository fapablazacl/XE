find_package(pugixml CONFIG REQUIRED)

set (target "xe-glaze-producer")

set (sources
    "src/xe/glaze/producer/ParseError.h"
    "src/xe/glaze/producer/ParseError.cpp"
    "src/xe/glaze/producer/RegistryLoader.h"
    "src/xe/glaze/producer/RegistryLoader.cpp"
    "src/xe/glaze/producer/detail/XmlHelpers.h"
    "src/xe/glaze/producer/detail/XmlHelpers.cpp"
    "src/xe/glaze/producer/detail/TypeParser.h"
    "src/xe/glaze/producer/detail/TypeParser.cpp"
    "src/xe/glaze/producer/detail/EnumParser.h"
    "src/xe/glaze/producer/detail/EnumParser.cpp"
    "src/xe/glaze/producer/detail/CommandParser.h"
    "src/xe/glaze/producer/detail/CommandParser.cpp"
    "src/xe/glaze/producer/detail/FeatureParser.h"
    "src/xe/glaze/producer/detail/FeatureParser.cpp"
    "src/xe/glaze/producer/detail/ExtensionParser.h"
    "src/xe/glaze/producer/detail/ExtensionParser.cpp"
)

add_library(${target} ${sources})
add_library(xe::glaze-producer ALIAS ${target})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target}
    PUBLIC  xe::glaze-model
    PRIVATE xe::interface
    PRIVATE pugixml::pugixml
)
