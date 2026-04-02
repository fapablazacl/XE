cmake_minimum_required(VERSION 3.15)
project(test_glaze CXX)

find_package(glaze REQUIRED)

add_executable(test_glaze test_glaze.cpp)
target_link_libraries(test_glaze PRIVATE glaze::gl)
