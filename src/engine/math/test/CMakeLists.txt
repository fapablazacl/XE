set (test_target "XE.Math.Test")

set (test_headers "GoogleTestCommon.h")

set (test_sources
        ${test_headers}
    "Box.Test.cpp"
    "Common.Test.cpp"
    "Ellipsoid.Test.cpp"
    "Main.cpp"
    "Matrix.Test.cpp"
    "Plane.Test.cpp"
    "Quaternion.Test.cpp"
    "Range.Test.cpp"
    "Ray.Test.cpp"
    "Rect.Test.cpp"
    "Rotation.Test.cpp"
    "Sphere.Test.cpp"
    "Triangle.Test.cpp"
    "Vector.Test.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} XE.Core XE.Interface gtest gmock)

# NOTE: The test name must have the same name as the command name
add_test(NAME ${test_target} COMMAND ${test_target})
