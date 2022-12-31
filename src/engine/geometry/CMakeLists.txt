
set (target XE.Geometry)

set (sources
    "src/xe/geometry/ShapeGenerator.h"
	"src/xe/geometry/EllipsoidGenerator.h"
	"src/xe/geometry/PlaneGenerator.h"
	"src/xe/geometry/BoxGenerator.h"
	"src/xe/geometry/ShapeGenerator.cpp"
	"src/xe/geometry/EllipsoidGenerator.cpp"
	"src/xe/geometry/BoxGenerator.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "src/")
target_link_libraries(${target} XE.Core)
