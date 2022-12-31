set (test_target "XE.Math.Test")

set (test_sources 
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

set (test_headers "Common.h" ${headers})

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} Catch2::Catch2WithMain XE.Core)

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
