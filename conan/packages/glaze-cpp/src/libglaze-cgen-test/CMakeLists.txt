set (target "glaze-cgen-test")

set (sources
    "CGenerator.Generate.Test.cpp"
)

add_executable(${target} ${sources})
# Reuse the MiniRegistry.h fixture from the producer test suite.
target_include_directories(${target} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/libglaze-producer-test"
)
target_link_libraries(${target} PRIVATE
    xe::glaze-cgen
    xe::glaze-producer
    xe::glaze-model
    xe::interface
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${target})
