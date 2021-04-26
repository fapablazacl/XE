
set (target XE.Math)

set (sources
    "include/XE/Math/Common.hpp"
    "include/XE/Math/Vector.hpp"
    "include/XE/Math/Matrix.hpp"
    "include/XE/Math/Quaternion.hpp"
    "include/XE/Math/Boundary.hpp"
    "include/XE/Math/Box.hpp"
    "include/XE/Math/Plane.hpp"
    "include/XE/Math/Ray.hpp"
    "include/XE/Math/Rect.hpp"
    "include/XE/Math/Sphere.hpp"
    "include/XE/Math/Triangle.hpp"
    "include/XE/Math/Ellipsoid.hpp"

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
