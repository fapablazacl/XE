cmake_minimum_required(VERSION 3.15)
project(test_glaze C CXX)

find_package(glaze REQUIRED)

# Build tests only for components that exist
if (TARGET glaze::gl)
    add_executable(test_gl_c test_gl_c.c)
    target_link_libraries(test_gl_c PRIVATE glaze::gl)

    if (EXISTS "${glaze_gl_INCLUDE_DIRS_RELEASE}/glaze/gl.hpp" OR EXISTS "${glaze_gl_INCLUDE_DIRS_DEBUG}/glaze/gl.hpp")
        add_executable(test_gl test_gl.cpp)
        target_link_libraries(test_gl PRIVATE glaze::gl)
    endif()
endif()

if (TARGET glaze::gles1)
    add_executable(test_gles1_c test_gles1_c.c)
    target_link_libraries(test_gles1_c PRIVATE glaze::gles1)

    if (EXISTS "${glaze_gles1_INCLUDE_DIRS_RELEASE}/glaze/gles1.hpp" OR EXISTS "${glaze_gles1_INCLUDE_DIRS_DEBUG}/glaze/gles1.hpp")
        add_executable(test_gles1 test_gles1.cpp)
        target_link_libraries(test_gles1 PRIVATE glaze::gles1)
    endif()
endif()

if (TARGET glaze::gles2)
    add_executable(test_gles2_c test_gles2_c.c)
    target_link_libraries(test_gles2_c PRIVATE glaze::gles2)

    if (EXISTS "${glaze_gles2_INCLUDE_DIRS_RELEASE}/glaze/gles2.hpp" OR EXISTS "${glaze_gles2_INCLUDE_DIRS_DEBUG}/glaze/gles2.hpp")
        add_executable(test_gles2 test_gles2.cpp)
        target_link_libraries(test_gles2 PRIVATE glaze::gles2)
    endif()
endif()

if (TARGET glaze::glsc2)
    add_executable(test_glsc2_c test_glsc2_c.c)
    target_link_libraries(test_glsc2_c PRIVATE glaze::glsc2)

    if (EXISTS "${glaze_glsc2_INCLUDE_DIRS_RELEASE}/glaze/glsc2.hpp" OR EXISTS "${glaze_glsc2_INCLUDE_DIRS_DEBUG}/glaze/glsc2.hpp")
        add_executable(test_glsc2 test_glsc2.cpp)
        target_link_libraries(test_glsc2 PRIVATE glaze::glsc2)
    endif()
endif()

# Single api-agnostic smoke test for the static glaze::Unique / Shared / Weak
# templates. Linked against the header-only glaze::raii component.
if (TARGET glaze::raii)
    add_executable(test_raii test_raii.cpp)
    target_link_libraries(test_raii PRIVATE glaze::raii)
endif()
