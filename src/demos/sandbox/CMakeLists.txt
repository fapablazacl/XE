
# CGLTF dependency
include_directories("dependencies/cgltf")

set (target "XE.Sandbox")
set (src 
	"src/Main.cpp"
	"src/Assets.cpp"
	"src/Assets.hpp"
	"src/SandboxApp.cpp"
	"src/SandboxApp.hpp"
	"src/Common.h"
    "src/Common.cpp"
	"src/AssetManager.cpp"
	"src/AssetManager.h"
	"src/Asset.cpp"
	"src/Asset.h"
	"src/Asset_CGLTF.cpp"
	"src/Asset_CGLTF.h"
	"src/Object.cpp"
	"src/Object.h"
	"src/Model.cpp"
	"src/Model.h"
	"src/Scene.h"
)

add_executable (${target} ${src})

target_link_libraries (${target} XE.Core XE.Graphics.GL XE.Platform.GLFW XE.Graphics.PNG)
