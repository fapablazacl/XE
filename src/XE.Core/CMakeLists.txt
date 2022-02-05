
set (target "XE.Core")

set (sources
	"include/XE/XE.h"
	"include/XE/Predef.h"
    "include/XE/Buffer.h"
    "include/XE/DataType.h"
	"include/XE/Core.h"
	"include/XE/Timer.h"
	
	"src/Buffer.cpp"
    "src/DataType.cpp"
	"src/Core.cpp"
	"src/Timer.cpp"
)

add_library (${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
