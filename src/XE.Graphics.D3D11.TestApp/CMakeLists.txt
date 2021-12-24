
set (target "XE.Graphics.D3D11.TestApp")
set (source_base "src")
set (sources
	"${source_base}/TestApp.cpp"
	"${source_base}/Renderer.h"
	"${source_base}/Renderer.cpp"
	"${source_base}/Mesh.h"
	"${source_base}/Mesh.cpp"
	"${source_base}/Shader.h"
	"${source_base}/Shader.cpp"
	"${source_base}/Color.vs"
	"${source_base}/Color.ps"
) 

add_executable(${target} ${sources})

target_link_libraries(${target} d3d11 d3dcompiler XE.Math)
