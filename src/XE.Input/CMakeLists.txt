
set (target XE.Input)

set (sources
    "include/XE/Input/InputManager.hpp"
	"include/XE/Input/DeviceStatus.hpp"

	"src/DeviceStatus.cpp"
    "src/InputManager.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
