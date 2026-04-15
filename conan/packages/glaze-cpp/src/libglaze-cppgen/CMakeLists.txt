find_package(inja CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include("${CMAKE_SOURCE_DIR}/cmake/EmbedTextFile.cmake")

set (target "glaze-cppgen")

set (generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
file (MAKE_DIRECTORY "${generated_dir}")

xe_glaze_embed_text_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/gl.hpp.inja"
    "${generated_dir}/gl_hpp_inja.cpp"
    "xe::glaze::cppgen::detail::templates"
    "gl_hpp_inja"
)
xe_glaze_embed_text_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/gl_handle.hpp.inja"
    "${generated_dir}/gl_handle_hpp_inja.cpp"
    "xe::glaze::cppgen::detail::templates"
    "gl_handle_hpp_inja"
)
xe_glaze_embed_text_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/raii.hpp"
    "${generated_dir}/raii_hpp.cpp"
    "xe::glaze::cppgen::detail::templates"
    "raii_hpp"
)

set (sources
    "src/xe/glaze/cppgen/CppGenerator.h"
    "src/xe/glaze/cppgen/CppGenerator.cpp"
    "src/xe/glaze/cppgen/detail/GroupRename.h"
    "src/xe/glaze/cppgen/detail/GroupRename.cpp"
    "src/xe/glaze/cppgen/detail/PatternDetector.h"
    "src/xe/glaze/cppgen/detail/PatternDetector.cpp"
    "src/xe/glaze/cppgen/detail/ParamFormat.h"
    "src/xe/glaze/cppgen/detail/ParamFormat.cpp"
    "src/xe/glaze/cppgen/detail/EnumClassEmitter.h"
    "src/xe/glaze/cppgen/detail/EnumClassEmitter.cpp"
    "src/xe/glaze/cppgen/detail/RaiiCollector.h"
    "src/xe/glaze/cppgen/detail/RaiiCollector.cpp"
    "src/xe/glaze/cppgen/detail/FunctorEmitter.h"
    "src/xe/glaze/cppgen/detail/FunctorEmitter.cpp"
    "src/xe/glaze/cppgen/detail/OverloadEmitter.h"
    "src/xe/glaze/cppgen/detail/OverloadEmitter.cpp"
    "src/xe/glaze/cppgen/detail/DsaEmitter.h"
    "src/xe/glaze/cppgen/detail/DsaEmitter.cpp"
    "src/xe/glaze/cppgen/detail/HandleEmitter.h"
    "src/xe/glaze/cppgen/detail/HandleEmitter.cpp"
    "src/xe/glaze/cppgen/detail/CppKeywords.h"
    "src/xe/glaze/cppgen/detail/CppKeywords.cpp"
    "${generated_dir}/gl_hpp_inja.cpp"
    "${generated_dir}/gl_handle_hpp_inja.cpp"
    "${generated_dir}/raii_hpp.cpp"
)

add_library(${target} ${sources})
add_library(xe::glaze-cppgen ALIAS ${target})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target}
    PUBLIC  xe::glaze-model
    PUBLIC  xe::glaze-docparser
    PRIVATE xe::interface
    PRIVATE pantor::inja
    PRIVATE nlohmann_json::nlohmann_json
)
