
set (target "xe-math")

set (headers
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
    "src/xe/math/VectorExpr.h")

# xe-math is now header-only. Exposing it as an INTERFACE library means
# consumers pick up the headers transitively without linking a separate .lib.
add_library(${target} INTERFACE)
add_library(xe::math ALIAS ${target})

target_sources(${target} INTERFACE FILE_SET HEADERS BASE_DIRS "src" FILES ${headers})
target_include_directories(${target} INTERFACE "src")
target_link_libraries(${target} INTERFACE xe::interface)

if (XE_DEV_UNIT_TEST)
    add_subdirectory("test")
endif()
