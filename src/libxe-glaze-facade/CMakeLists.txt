find_package(cxxopts CONFIG REQUIRED)

set (target "xe-glaze-facade")

set (sources
    "src/xe/glaze/Glaze.h"
    "src/xe/glaze/Language.h"
    "src/xe/glaze/GenerateOptions.h"
    "src/xe/glaze/BindingGenerator.h"
    "src/xe/glaze/BindingGenerator.cpp"
    "src/xe/glaze/cli/ArgParse.h"
    "src/xe/glaze/cli/ArgParse.cpp"
)

add_library(${target} ${sources})
add_library(xe::glaze-facade ALIAS ${target})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target}
    PUBLIC  xe::glaze-model
    PUBLIC  xe::glaze-producer
    PUBLIC  xe::glaze-cgen
    PUBLIC  xe::glaze-cppgen
    PUBLIC  cxxopts::cxxopts
    PRIVATE xe::interface
)
