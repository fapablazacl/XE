
set (target "graphics-demo")

set (sources 
	"Camera.cpp" 
	"Camera.h" 
	"Common.cpp"
	"Common.h"
	"Demo.cpp"
	"Platform.cpp"
	"Platform.h"
	"Renderable.h"
	"SceneNode.cpp"
	"SceneNode.h"
	"Util.cpp"
	"Util.h" 
)

add_executable(${target} ${sources})

target_include_directories(${target} PUBLIC ".")

target_link_libraries(${target} 
	XE.Core 
	XE.Math 
	XE.IO 
	XE.Graphics 
	XE.Input 
	XE.Graphics.GL 
	XE.Graphics.ES2
	XE.Graphics.GL.GLFW)

