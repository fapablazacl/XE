
set (target glad-es2)

set (sources 
	"include/glad/es2/glad.h"
	"include/glad/es2/glad_debug.h"
	"include/glad/es2/glad_release.h"

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
