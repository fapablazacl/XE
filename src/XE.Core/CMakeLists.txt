
set (target "XE.Core")

set (sources
	"include/XE/XE.hpp"
	"include/XE/Graphics.hpp"
	"include/XE/Math.hpp"
	"include/XE/Input.hpp"
	"include/XE/Predef.hpp"
    "include/XE/Buffer.hpp"
    "include/XE/DataType.hpp"
	"include/XE/Core.hpp"
    "include/XE/Frame.hpp"
	"include/XE/WindowSystem.hpp"
	"include/XE/Timer.hpp"
	
	"src/Buffer.cpp"
    "src/DataType.cpp"
	"src/Core.cpp"
	"src/Frame.cpp"
	"src/WindowSystem.cpp"
	"src/Timer.cpp"
)

add_library (${target} ${sources})

target_include_directories(${target} PUBLIC "include/")

target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})

#if (UNIX)
#	target_link_libraries(${target} stdc++fs)
#endif()
