
cmake_minimum_required (VERSION 3.16)

project (XE)

option (XE_PLUGIN_PNG           "Include PNG image support via LodePNG" OFF)
option (XE_PLUGIN_DIRECT3D11    "Include Direct3D 11 support" OFF)
option (XE_PLUGIN_GL            "Include OpenGL 3+ support" OFF)
option (XE_PLUGIN_ES2           "Include OpenGL ES 2 support" OFF)
option (XE_PLUGIN_GL_GLFW       "Include GLFW-based Window and Context creation support (requires 'XE_PLUGIN_GL')" OFF)
option (XE_PLUGIN_VULKAN        "Include Vulkan support" OFF)

option (XE_EXAMPLE_SANDBOX      "Include example program that is used for various development activities" OFF)

# options suited for development
option (XE_DEV_UNIT_TEST            "Enable unit test suite" ON)
option (XE_DEV_BENCHMARK_TEST       "Enable benchmark tests" OFF)
option (XE_DEV_WARNINGS_AS_ERRORS   "Marks all warnings as errors" OFF)
option (XE_DEV_INSTRUMENT_COVERAGE  "Intrument code for code coverage reporting during unit tests" OFF)
option (XE_DEV_PRECOMPILED_HEADERS  "Enable precompiled headers for internal libraries, to improve build times" OFF)
option (XE_DEV_LINT_CLANG_TIDY      "Enable linting checks with clang-tidy" OFF)
option (XE_DEV_IWYU                 "Enable include checks with iwyu" OFF)

set (CMAKE_CXX_STANDARD             17)
set (CMAKE_CXX_STANDARD_REQUIRED    ON)
set (CMAKE_CXX_EXTENSIONS           OFF)

# TODO: Configure the version numbering mechanism
set (XE_VERSION_MAJOR 1)
set (XE_VERSION_MINOR 0)
set (XE_VERSION_REVISION 0)
set (XE_VERSION "${XE_VERSION_MAJOR}.${XE_VERSION_MINOR}.${XE_VERSION_REVISION}")

if (XE_DEV_UNIT_TEST)
    include(CTest)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    # this macro is required by Glad.
    add_definitions (-D_DEBUG)
endif ()

add_definitions (-DXE_VERSION_MAJOR=${XE_VERSION_MAJOR})
add_definitions (-DXE_VERSION_MINOR=${XE_VERSION_MINOR})
add_definitions (-DXE_VERSION_REVISION=${XE_VERSION_REVISION})

set (target XE.Interface)
add_library(${target} INTERFACE)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set (XE_CXXFLAGS_WALL -Wall -Wextra)
    set (XE_CXXFLAGS_WERROR -Werror)
    set (XE_CXXFLAGS_COVERAGE -fprofile-arcs -ftest-coverage)
    set (XE_LDFLAGS_COVERAGE )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (XE_CXXFLAGS_WALL -Wall -Wextra)
    set (XE_CXXFLAGS_WERROR -Werror)
    set (XE_CXXFLAGS_COVERAGE -fprofile-instr-generate -fcoverage-mapping)
    set (XE_LDFLAGS_COVERAGE -fprofile-instr-generate)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set (XE_CXXFLAGS_WALL /W4)
    set (XE_CXXFLAGS_WERROR /WX)
    set (XE_CXXFLAGS_COVERAGE )
    set (XE_LDFLAGS_COVERAGE )
endif()

target_compile_definitions(${target} INTERFACE _CRT_SECURE_NO_WARNINGS)

target_compile_options(${target} INTERFACE ${XE_CXXFLAGS_WALL})
if (XE_DEV_WARNINGS_AS_ERRORS)
    target_compile_options(${target} INTERFACE ${XE_CXXFLAGS_WERROR})
endif()
if (XE_DEV_INSTRUMENT_COVERAGE)
    target_compile_options(${target} INTERFACE ${XE_CXXFLAGS_COVERAGE})
    target_link_options(${target} INTERFACE ${XE_LDFLAGS_COVERAGE})
endif() 

if (XE_DEV_LINT_CLANG_TIDY)
    set_target_properties(${target} PROPERTIES CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

if (XE_DEV_IWYU)
    set_target_properties(${target} PROPERTIES CMAKE_CXX_INCLUDE_WHAT_YOU_USE "iwyu")
endif()

add_subdirectory ("src")
