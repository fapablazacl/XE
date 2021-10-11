
set (target "XE.Scene")

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


# Unit Test 
set (test_target "XE.Scene.Test")

set (test_sources
    "test/Main.cpp"
    "test/Projection.Test.cpp"
    "test/Trackball.Test.cpp"
    "test/VirtualSphere.Test.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} XE.Core XE.Scene)

# NOTE: The test name must have the same name as the command name
add_test(NAME ${test_target} COMMAND ${test_target})
