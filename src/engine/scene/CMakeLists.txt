
set (target "XE.Scene")
set (sources
	"src/xe/scene/Projection.h"
	"src/xe/scene/Trackball.h"
    "src/xe/scene/VirtualSphere.h"
	"src/xe/scene/Trackball.cpp"
	"src/xe/scene/VirtualSphere.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "src/")
target_link_libraries(${target} PUBLIC XE.Core PRIVATE XE.Interface)

if (XE_DEV_UNIT_TEST)
    set (test_target "XE.Scene.Test")

    set (test_sources
        "test/Main.cpp"
        "test/Projection.Test.cpp"
        "test/Trackball.Test.cpp"
        "test/VirtualSphere.Test.cpp"
    )

    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target} gtest XE.Scene XE.Interface)

    # NOTE: The test name must have the same name as the command name
    add_test(NAME ${test_target} COMMAND ${test_target})
endif()
