
set (target XE.Geometry)

set (sources
    "include/XE/Geometry/ShapeGenerator.h"
	"include/XE/Geometry/EllipsoidGenerator.h"
	"include/XE/Geometry/PlaneGenerator.h"
	"include/XE/Geometry/BoxGenerator.h"

	"src/ShapeGenerator.cpp"
	"src/EllipsoidGenerator.cpp"
	"src/BoxGenerator.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
