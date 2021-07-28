add_compile_options(${XE_CXX_FLAGS})

set (target "XE.Core")

set (sources
	"include/XE/XE.h"
	"include/XE/Graphics.h"
	"include/XE/Math.h"
	"include/XE/Input.h"
	"include/XE/Predef.h"
    "include/XE/Buffer.h"
    "include/XE/DataType.h"
	"include/XE/Core.h"
    "include/XE/Frame.h"
	"include/XE/WindowSystem.h"
	"include/XE/Timer.h"
	
	"src/Buffer.cpp"
    "src/DataType.cpp"
	"src/Core.cpp"
	"src/Frame.cpp"
	"src/WindowSystem.cpp"
	"src/Timer.cpp"
)

add_library (${target} ${sources})

target_include_directories(${target} PUBLIC "include/")

#target_compile_options (${target} PUBLIC "${XE_CXX_FLAGS}")

#if (UNIX)
#	target_link_libraries(${target} stdc++fs)
#endif()
