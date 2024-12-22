
# CGLTF dependency

hunter_add_package(cgltf)
find_package(cgltf CONFIG REQUIRED)

hunter_add_package(SDL2)
find_package(SDL2 CONFIG REQUIRED)


set (target "capybaria")
set (src
	"src/sandbox_main.cpp"
		"src/Common.h"
    "src/Common.cpp"
	"src/Asset_CGLTF.cpp"
	"src/Asset_CGLTF.h"
		"src/Util.cpp"
	"src/Util.h"
		"src/RendererGL.cpp"
	"src/RendererGL.h")

add_executable (${target} ${src} )

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
target_compile_definitions(${target} PRIVATE XE_SANDBOX_ROOT_PATH="${PARENT_DIR}/sandbox")
target_link_libraries (${target} XE.Interface XE.Core XE.Graphics.GL XE.Platform.GLFW XE.Graphics.PNG cgltf::cgltf SDL2::SDL2 SDL2::SDL2main)
