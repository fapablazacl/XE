
set (target XE.Geometry)

set (sources
    "include/XE/Geometry/ShapeGenerator.hpp"
	"include/XE/Geometry/EllipsoidGenerator.hpp"
	"include/XE/Geometry/PlaneGenerator.hpp"
	"include/XE/Geometry/BoxGenerator.hpp"

	"src/ShapeGenerator.cpp"
	"src/EllipsoidGenerator.cpp"
	"src/PlaneGenerator.cpp"
	"src/BoxGenerator.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
