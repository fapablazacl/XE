
set (target "XE.Core")

set (headers 
	"include/XE/XE.h"
	"include/XE/Predef.h"
	"include/XE/Buffer.h"
	"include/XE/DataType.h"
	"include/XE/Core.h"
	"include/XE/Timer.h"
)

set (sources
	${headers}

	"src/Buffer.cpp"
    "src/DataType.cpp"
	"src/Core.cpp"
	"src/Timer.cpp"
)

add_library (${target} ${sources})

target_precompile_headers(${target} PRIVATE ${headers})
target_include_directories(${target} PUBLIC "include/")
target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${target} PRIVATE -Wall -Wextra)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Werror)
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${target} PRIVATE /Wall)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE /WX)
    endif()
endif()
