
include_directories(.)

set (sources 
	glad/es2/glad.c
	glad/es2/glad_debug.c.in
	glad/es2/glad_release.c.in
	glad/es2/glad.h
	glad/es2/glad_debug.h
	glad/es2/glad_release.h
)

set (target glad-es2)

source_group(\\glad FILES ${sources})

if (UNIX)
	set (OS_LIBRARIES dl)
endif ()

add_library(${target} ${sources})

target_link_libraries(${target} ${OS_LIBRARIES})
