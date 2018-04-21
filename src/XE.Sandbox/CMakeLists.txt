
set (target "XE.Sandbox")
set (src 
	"Main.cpp"
	"Assets.cpp"
	"Assets.hpp"
	"SandboxApp.cpp"
	"SandboxApp.hpp"
)

source_group ("\\" FILES ${src})

add_executable (${target} ${src})

target_link_libraries (${target} XE.Core XE.Graphics.OpenGL)
