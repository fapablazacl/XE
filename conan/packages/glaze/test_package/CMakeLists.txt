cmake_minimum_required(VERSION 3.15)
project(test_glaze CXX)

find_package(glaze REQUIRED)

add_executable(test_gl test_gl.cpp)
target_link_libraries(test_gl PRIVATE glaze::gl)

add_executable(test_gles1 test_gles1.cpp)
target_link_libraries(test_gles1 PRIVATE glaze::gles1)

add_executable(test_gles2 test_gles2.cpp)
target_link_libraries(test_gles2 PRIVATE glaze::gles2)

add_executable(test_glsc2 test_glsc2.cpp)
target_link_libraries(test_glsc2 PRIVATE glaze::glsc2)
