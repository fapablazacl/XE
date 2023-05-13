
# CGLTF dependency
include_directories("dependencies/cgltf")

set (target "XE.Sandbox")
set (src 
	"src/ecs/ECS.h"
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
	"src/Components.h"
	"src/PhysicsSystem.h"
	"src/PhysicsSystem.cpp"
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
		src/FPSCounter.cpp src/FPSCounter.h src/DemoApp.h src/DemoRenderable.cpp src/DemoRenderable.h ../../engine/core/src/xe/Flags.h src/ecs/MessageBus.cpp src/ecs/MessageBus.h src/ecs/EntityManager.cpp src/ecs/EntityManager.h src/ecs/ComponentArray.cpp src/ecs/ComponentArray.h src/ecs/ComponentManager.cpp src/ecs/ComponentManager.h src/ecs/System.cpp src/ecs/System.h src/ecs/SystemManager.cpp src/ecs/SystemManager.h src/ecs/Coordinator.cpp src/ecs/Coordinator.h src/ecs/CommonECS.h src/InputSystem.cpp src/InputSystem.h)

add_executable (${target} ${src})

target_link_libraries (${target} XE.Interface XE.Core XE.Graphics.GL XE.Platform.GLFW XE.Graphics.PNG)
