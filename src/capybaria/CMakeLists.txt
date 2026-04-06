
find_package(cgltf CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set (target "capybaria")
set (src
	"src/capybaria_main.cpp"
	"src/Common.h"
	"src/Common.cpp"
	"src/Game.h"
	"src/Game.cpp"
	"src/Asset_CGLTF.cpp"
	"src/Asset_CGLTF.h"
	"src/Util.cpp"
	"src/Util.h"
)

add_executable (${target} ${src} )

get_filename_component(PARENT_DIR "." DIRECTORY)
target_compile_definitions(${target} PRIVATE CAPYBARIA_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries (${target} xe::interface xe::core xe::gl xe::platform-glfw xe::graphics-png cgltf::cgltf SDL2::SDL2 SDL2::SDL2main)
