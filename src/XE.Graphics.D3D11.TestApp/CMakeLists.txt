
set (target "XE.Graphics.D3D11.TestApp")
set (source_base "XE/Graphics/D3D11/TestApp")
set (sources_xe_graphics_d3d11_testapp
	"${source_base}/TestApp.cpp"
	"${source_base}/Renderer.hpp"
	"${source_base}/Renderer.cpp"
	"${source_base}/Mesh.hpp"
	"${source_base}/Mesh.cpp"
	"${source_base}/Shader.hpp"
	"${source_base}/Shader.cpp"
	"${source_base}/Color.vs"
	"${source_base}/Color.ps"
) 

source_group("\\XE\\Graphics\\D3D11\\TestApp" FILES ${sources_xe_graphics_d3d11_testapp})

add_executable(${target} ${sources_xe_graphics_d3d11_testapp})

target_link_libraries(${target} d3d11 d3dcompile)
