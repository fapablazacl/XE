
set (target "XE.Math")

set (headers
        "include/xe/math/Math.h"
        "include/xe/math/Math.h"
        "include/xe/math/Common.h"
        "include/xe/math/Vector.h"
        "include/xe/math/Matrix.h"
        "include/xe/math/Quaternion.h"
        "include/xe/math/Boundary.h"
        "include/xe/math/Box.h"
        "include/xe/math/Plane.h"
        "include/xe/math/Ray.h"
        "include/xe/math/Rect.h"
        "include/xe/math/Sphere.h"
        "include/xe/math/Triangle.h"
        "include/xe/math/Ellipsoid.h"
)

set (sources
        ${headers}
        src/Quaternion.cpp
        src/Vector.cpp
        src/Matrix.cpp src/Box.cpp src/Plane.cpp src/Range.cpp src/Ray.cpp src/Rect.cpp src/Ellipsoid.cpp src/Sphere.cpp src/Triangle.cpp)

add_library (${target} ${sources})

if (XE_DEV_PRECOMPILED_HEADERS)
    target_precompile_headers(${target} PRIVATE ${headers})
endif()

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} PRIVATE XE.Interface)

if (XE_DEV_UNIT_TEST)
    add_subdirectory("test")
endif()
