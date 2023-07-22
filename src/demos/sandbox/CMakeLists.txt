
# CGLTF dependency
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")

set (target "XE.Sandbox")
set (src
	"src/sandbox_main.cpp"
	"src/Assets.cpp"
	"src/Assets.h"
	"src/SandboxApp.cpp"
	"src/SandboxApp.h"
	"src/Common.h"
    "src/Common.cpp"
	"src/Asset_CGLTF.cpp"
	"src/Asset_CGLTF.h"
	"src/Scene.h"
	"src/RenderingSystem.h"
	"src/RenderingSystem.cpp"
	"src/Camera.cpp" 
	"src/Camera.h" 
	"src/DemoApp.cpp"
	"src/Renderable.h"
	"src/SceneNode.cpp"
	"src/SceneNode.h"
	"src/Util.cpp"
	"src/Util.h"
	"src/AssetManager.h"
	"src/AssetManager.cpp"
	"src/DemoApp.h" 
	"src/DemoRenderable.cpp" 
	"src/DemoRenderable.h" 
	"src/Logger.cpp" 
	"src/Logger.h")

add_executable (${target} ${src} )

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
target_compile_definitions(${target} PRIVATE XE_SANDBOX_ROOT_PATH="${PARENT_DIR}/sandbox")
target_include_directories(${target} PRIVATE ${CGLTF_INCLUDE_DIRS})
target_link_libraries (${target} XE.Interface XE.Core XE.Graphics.GL XE.Platform.GLFW XE.Graphics.PNG)
