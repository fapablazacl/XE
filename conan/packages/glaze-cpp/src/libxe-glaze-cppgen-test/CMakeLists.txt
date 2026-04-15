set (target "xe-glaze-cppgen-test")

set (sources
    "CppGenerator.Generate.Test.cpp"
    "CppGenerator.Extensions.Test.cpp"
    "CppGenerator.Overloads.Test.cpp"
    "CppKeywords.Test.cpp"
    "PatternDetector.Test.cpp"
)

add_executable(${target} ${sources})
# Reuse the MiniRegistry.h fixture from the producer test suite.
target_include_directories(${target} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/libxe-glaze-producer-test"
)
target_link_libraries(${target} PRIVATE
    xe::glaze-cppgen
    xe::glaze-producer
    xe::glaze-model
    xe::interface
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${target})
