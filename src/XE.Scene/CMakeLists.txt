
set (target XE.Scene)

set (sources
	"include/XE/Scene/Projection.hpp"
	"include/XE/Scene/Trackball.hpp"
    "include/XE/Scene/VirtualSphere.hpp"

    "src/Projection.cpp"
	"src/Trackball.cpp"
	"src/VirtualSphere.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Graphics XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
