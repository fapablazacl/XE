
cmake_minimum_required (VERSION 3.16)

set (CMAKE_DEBUG_POSTFIX "d")

project (glad-gl4)

# Target Definition
set (target glad-gl4)

set (sources 
	"include/glad/glad.h"
	"include/glad/glad_release.h"
	"include/glad/glad_debug.h"
    "src/glad.c"
	"src/glad_release_impl.h"
	"src/glad_debug_impl.h"
)

add_library (${target} ${sources})
target_include_directories(${target} PUBLIC "include")
