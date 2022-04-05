
set (target XE.Math)

set (headers
    "include/XE/Math.h"
    "include/XE/Math/Common.h"
    "include/XE/Math/Vector.h"
    "include/XE/Math/Matrix.h"
    "include/XE/Math/Quaternion.h"
    "include/XE/Math/Boundary.h"
    "include/XE/Math/Box.h"
    "include/XE/Math/Plane.h"
    "include/XE/Math/Ray.h"
    "include/XE/Math/Rect.h"
    "include/XE/Math/Sphere.h"
    "include/XE/Math/Triangle.h"
    "include/XE/Math/Ellipsoid.h"
)

set (sources
    ${headers}
)

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE "include")


if (XE_UNIT_TEST)
    set (test_target "XE.Math.Test")

    set (test_sources 
        "test/Box.Test.cpp"
        "test/Common.Test.cpp"
        "test/Ellipsoid.Test.cpp"
        "test/Main.cpp"
        "test/Matrix.Test.cpp"
        "test/Plane.Test.cpp"
        "test/Quaternion.Test.cpp"
        "test/Range.Test.cpp"
        "test/Ray.Test.cpp"
        "test/Rect.Test.cpp"
        "test/Rotation.Test.cpp"
        "test/Sphere.Test.cpp"
        "test/Triangle.Test.cpp"
        "test/Vector.Test.cpp"
    )

    set (test_headers "test/Common.h" ${headers})

    add_executable(${test_target} ${test_sources})
    target_precompile_headers(${test_target} PRIVATE ${test_headers})
    target_link_libraries(${test_target} Catch2::Catch2WithMain XE.Math)

    # Enable all warnings
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${test_target} PRIVATE -Wall -Wextra)

        if (XE_WARNINGS_AS_ERRORS)
            target_compile_options(${test_target} PRIVATE -Werror)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${test_target} PRIVATE /Wall)

        if (XE_WARNINGS_AS_ERRORS)
            target_compile_options(${test_target} PRIVATE /WX)
        endif()
    endif()

    # NOTE: The test name must have the same name as the command name
    add_test(NAME ${test_target} COMMAND ${test_target})
endif()
