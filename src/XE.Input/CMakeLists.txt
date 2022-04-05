
set (target XE.Input)

set (headers
    "include/XE/Input.h"
    "include/XE/Input/InputManager.h"
    "include/XE/Input/DeviceStatus.h"
)

set (sources
    ${headers}
	"src/DeviceStatus.cpp"
    "src/InputManager.cpp"
)

add_library(${target} ${sources})

target_precompile_headers(${target} PRIVATE ${headers})
target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core XE.Math)
