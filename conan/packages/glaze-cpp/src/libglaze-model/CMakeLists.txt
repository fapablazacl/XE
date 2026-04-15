set (target "glaze-model")

set (sources
    "src/xe/glaze/model/ApiProfile.h"
    "src/xe/glaze/model/TypeDecl.h"
    "src/xe/glaze/model/TypeDecl.cpp"
    "src/xe/glaze/model/Type.h"
    "src/xe/glaze/model/Enum.h"
    "src/xe/glaze/model/EnumGroup.h"
    "src/xe/glaze/model/EnumGroup.cpp"
    "src/xe/glaze/model/CommandParam.h"
    "src/xe/glaze/model/CommandParam.cpp"
    "src/xe/glaze/model/Command.h"
    "src/xe/glaze/model/Command.cpp"
    "src/xe/glaze/model/Feature.h"
    "src/xe/glaze/model/Extension.h"
    "src/xe/glaze/model/ConsolidatedRequire.h"
    "src/xe/glaze/model/StringUtils.h"
    "src/xe/glaze/model/StringUtils.cpp"
    "src/xe/glaze/model/Registry.h"
    "src/xe/glaze/model/Registry.cpp"
    "src/xe/glaze/codegen/NameTransform.h"
    "src/xe/glaze/codegen/NameTransform.cpp"
    "src/xe/glaze/codegen/TypeMapper.h"
    "src/xe/glaze/codegen/TypeMapper.cpp"
    "src/xe/glaze/codegen/ExtensionFilter.h"
    "src/xe/glaze/codegen/ExtensionFilter.cpp"
)

add_library(${target} ${sources})
add_library(xe::glaze-model ALIAS ${target})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target} PRIVATE xe::interface)
