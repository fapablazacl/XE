set (target "xe-math-test")

# Matrix.Layout.Test.cpp cross-checks xe::mat4 against glm::mat4. Even though
# glm is already available repo-wide via Conan, we explicitly find_package it
# here so the test CMakeLists is self-contained.
# Also, we will be using glm here to check if we are getting 
# correct results during our tests.
find_package(glm CONFIG REQUIRED)

# Catch2 v3 provides its own main via the WithMain target.
set (sources
    "Common.Test.cpp"
    "Vector.Test.cpp"
    "Vector.Xcheck.Test.cpp"
    "Vector.EdgeCases.Test.cpp"
    "Matrix.Test.cpp"
    "Matrix.Layout.Test.cpp"
    "Matrix.Xcheck.Test.cpp"
    "Matrix.EdgeCases.Test.cpp"
    "Quaternion.Test.cpp"
    "Quaternion.Xcheck.Test.cpp"
    "Quaternion.EdgeCases.Test.cpp"
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

add_executable(${target} ${sources})
target_link_libraries(${target} PRIVATE
    xe::math
    xe::interface
    Catch2::Catch2WithMain
    glm::glm  # used by Matrix.layout.test.cpp to cross-check memory layout
)

# Let CTest discover each TEST_CASE individually.
include(Catch)
catch_discover_tests(${target})
