
set (target glad-gl4)

set (sources 
	"include/glad/glad.h"
	"include/glad/glad_debug.h"
	"include/glad/glad_release.h"

	"src/glad.c"
	"src/glad_debug.c.in"
	"src/glad_release.c.in"
)

source_group (\\glad FILES ${sources})

if (UNIX)
	set (OS_LIBRARIES dl)
endif ()

add_library (${target} ${sources})

target_include_directories (${target} PUBLIC "include/")
target_link_libraries (${target} ${OS_LIBRARIES})
