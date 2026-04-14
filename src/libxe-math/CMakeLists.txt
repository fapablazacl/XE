
set (target "xe-math")

set (sources
    "src/xe/math/Math.h"
    "src/xe/math/Common.h"
    "src/xe/math/Vector.h"
    "src/xe/math/Matrix.h"
    "src/xe/math/Quaternion.h"
    "src/xe/math/Boundary.h"
    "src/xe/math/Box.h"
    "src/xe/math/Plane.h"
    "src/xe/math/Ray.h"
    "src/xe/math/Rect.h"
    "src/xe/math/Sphere.h"
    "src/xe/math/Triangle.h"
    "src/xe/math/Ellipsoid.h"
    "src/xe/math/Rotation.h"
    "src/xe/math/Range.h"
    "src/xe/math/Legacy.h"
    "src/xe/math/VectorExpr.h"
    "src/xe/math/foo.cpp")

add_library(${target} ${sources})
add_library(xe::math ALIAS ${target})
target_include_directories(${target} PUBLIC "src")
