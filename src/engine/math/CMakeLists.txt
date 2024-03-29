
set (target "XE.Math")

set (headers
    "src/xe/math/Math.h"
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
    "src/xe/math/VectorExpr.h")

set (sources
    ${headers}
    src/xe/math/Quaternion.cpp
    src/xe/math/Vector.cpp
    src/xe/math/Matrix.cpp
    src/xe/math/Box.cpp
    src/xe/math/Plane.cpp
    src/xe/math/Range.cpp
    src/xe/math/Ray.cpp
    src/xe/math/Rect.cpp
    src/xe/math/Ellipsoid.cpp
    src/xe/math/Sphere.cpp
    src/xe/math/Triangle.cpp)

add_library (${target} ${sources})

if (XE_DEV_PRECOMPILED_HEADERS)
    target_precompile_headers(${target} PRIVATE ${headers})
endif()

target_include_directories(${target} PUBLIC "src/")
target_link_libraries(${target} PRIVATE XE.Interface)

if (XE_DEV_UNIT_TEST)
    add_subdirectory("test")
endif()

if (XE_DEV_BENCHMARK_TEST)
    add_subdirectory("benchmark-test")
endif()
