
include_directories(.)

set (sources 
	glad/glad.c
	glad/glad_debug.c.in
	glad/glad_release.c.in
	glad/glad.h
	glad/glad_debug.h
	glad/glad_release.h
)

set (target glad-gl4)

source_group(\\glad FILES ${sources})

if (UNIX)
	set (OS_LIBRARIES dl)
endif ()

add_library(${target} ${sources})

target_link_libraries(${target} ${OS_LIBRARIES})
