
set (target XE.Math)

set (sources
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

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE "include/")


if (XE_UNIT_TEST)
    set (test_target "XE.Math.Test")

    set (test_sources 
        "test/Main.cpp"
        "test/Common.Test.cpp"
        "test/Vector.Test.cpp"
        "test/Matrix.Test.cpp"
        "test/Quaternion.Test.cpp"
    )

    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target} Catch2::Catch2 XE.Math)

    # NOTE: The test name must have the same name as the command name
    add_test(NAME ${test_target} COMMAND ${test_target})
endif()
