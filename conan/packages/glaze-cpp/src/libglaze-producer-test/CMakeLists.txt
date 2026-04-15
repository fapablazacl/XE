set (target "glaze-producer-test")

set (sources
    "MiniRegistry.h"
    "RegistryLoader.LoadFromString.Test.cpp"
    "RegistryLoader.LoadFromFile.Test.cpp"
    "RegistryLoader.Types.Test.cpp"
    "RegistryLoader.Enums.Test.cpp"
    "RegistryLoader.Commands.Test.cpp"
    "RegistryLoader.Features.Test.cpp"
    "RegistryLoader.Extensions.Test.cpp"
)

add_executable(${target} ${sources})
target_include_directories(${target} PRIVATE ".")
target_link_libraries(${target} PRIVATE
    xe::glaze-producer
    xe::glaze-model
    xe::interface
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${target})
