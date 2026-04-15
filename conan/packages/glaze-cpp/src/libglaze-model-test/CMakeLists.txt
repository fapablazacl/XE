set (target "glaze-model-test")

set (sources
    "StringUtils.Test.cpp"
    "TypeDecl.Test.cpp"
    "CommandParam.Test.cpp"
    "Command.Test.cpp"
    "Registry.Test.cpp"
    "NameTransform.Test.cpp"
    "TypeMapper.Test.cpp"
)

add_executable(${target} ${sources})
target_link_libraries(${target} PRIVATE
    xe::glaze-model
    xe::interface
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${target})
