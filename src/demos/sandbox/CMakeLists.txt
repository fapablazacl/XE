
# CGLTF dependency
include_directories("dependencies/cgltf")

set (target "XE.Sandbox")
set (src 
	"src/ECS.h"
	"src/Main.cpp"
	"src/Assets.cpp"
	"src/Assets.h"
	"src/SandboxApp.cpp"
	"src/SandboxApp.h"
	"src/Common.h"
    "src/Common.cpp"
	"src/AssetManager.cpp"
	"src/AssetManager.h"
	"src/Asset.cpp"
	"src/Asset.h"
	"src/Asset_CGLTF.cpp"
	"src/Asset_CGLTF.h"
	"src/Scene.h"
	"src/PhysicsSystem.h"
	"src/PhysicsSystem.cpp"
)

add_executable (${target} ${src})

target_link_libraries (${target} XE.Interface XE.Core XE.Graphics.GL XE.Platform.GLFW XE.Graphics.PNG)
