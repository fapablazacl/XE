
set (target "XE.Sandbox")
set (src 
	"Main.cpp"

	"Assets.cpp"
	"Assets.hpp"

	"SandboxApp.cpp"
	"SandboxApp.hpp"

	"Common.h"

	"AssetManager.cpp"
	"AssetManager.h"
	
	"Asset.cpp"
	"Asset.h"

	"Asset_CGLTF.cpp"
	"Asset_CGLTF.h"
	
	"Object.cpp"
	"Object.h"
	
	"Scene.h"
)

source_group ("\\" FILES ${src})

add_executable (${target} ${src})

target_link_libraries (${target} XE.Core XE.IO XE.Graphics.GL XE.Graphics.GL.GLFW XE.Graphics.PNG)
