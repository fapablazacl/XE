cmake_minimum_required(VERSION 3.16)
project(test_glazed C CXX)

find_package(glazed REQUIRED)

# Smoke test: include the prebuilt gl.hpp under a specific GL version macro
# and link against glazed::gl. This proves that:
#   1. find_package(glazed) loads the cmake_build_modules and creates targets
#   2. The prebuilt headers are reachable
#   3. The loader source `src/gl.c` compiles into the consumer's build via
#      target_sources INTERFACE
#   4. The compile-time version-gating macro flows through correctly
add_executable(test_glazed_gl test_glazed_gl.cpp)
target_link_libraries(test_glazed_gl PRIVATE glazed::gl)
target_compile_definitions(test_glazed_gl PRIVATE GLAZE_GL_VERSION=33)
