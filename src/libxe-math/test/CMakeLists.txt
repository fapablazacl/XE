set (test_target "xe-math-test")

# Matrix.Layout.Test.cpp cross-checks xe::mat4 against glm::mat4. Even though
# glm is already available repo-wide via Conan, we explicitly find_package it
# here so the test CMakeLists is self-contained.
find_package(glm CONFIG REQUIRED)

# Catch2 v3 provides its own main via the WithMain target.
set (test_sources
    "Common.Test.cpp"
    "Vector.Test.cpp"
    "Matrix.Test.cpp"
    "Matrix.Layout.Test.cpp"
    "Quaternion.Test.cpp"
    "Ray.Test.cpp"
    "Sphere.Test.cpp"
    "Plane.Test.cpp"
    "Triangle.Test.cpp"
    "Box.Test.cpp"
    "Ellipsoid.Test.cpp"
    "Rect.Test.cpp"
    "Range.Test.cpp"
    "Rotation.Test.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} PRIVATE
    xe::math
    xe::interface
    Catch2::Catch2WithMain
    glm::glm  # used by Matrix.layout.test.cpp to cross-check memory layout
)

# Let CTest discover each TEST_CASE individually.
include(Catch)
catch_discover_tests(${test_target})
