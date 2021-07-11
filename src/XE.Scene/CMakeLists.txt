
set (target XE.Scene)

set (sources
	"include/XE/Scene/Projection.h"
	"include/XE/Scene/Trackball.h"
    "include/XE/Scene/VirtualSphere.h"
	"include/XE/Scene/Scene.h"
    
    "src/Projection.cpp"
	"src/Trackball.cpp"
	"src/VirtualSphere.cpp"
	"src/Scene.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Graphics XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
