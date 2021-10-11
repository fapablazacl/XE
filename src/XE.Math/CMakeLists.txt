
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

    "src/Common.cpp"
    "src/Vector.cpp"
    "src/Matrix.cpp"
	"src/Quaternion.cpp"
	"src/Boundary.cpp"
	"src/Box.cpp"
	"src/Plane.cpp"
	"src/Ray.cpp"
	"src/Rect.cpp"
	"src/Sphere.cpp"
	"src/Triangle.cpp"
    "src/Ellipsoid.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})


# Unit Test 
set (test_target "XE.Math.Test")

set (test_sources 
    "test/Main.cpp"
    "test/Common.Test.cpp"
    "test/Vector.Test.cpp"
    "test/Matrix.Test.cpp"
    "test/Quaternion.Test.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} XE.Math)
