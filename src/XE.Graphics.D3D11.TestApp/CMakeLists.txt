
set (target "XE.Graphics.D3D11.TestApp")
set (sources_xe_graphics_d3d11_testapp
	"XE/Graphics/D3D11/TestApp/TestApp.cpp"
	"XE/Graphics/D3D11/TestApp/Renderer.hpp"
	"XE/Graphics/D3D11/TestApp/Renderer.cpp"
) 

source_group("\\XE\\Graphics\\D3D11\\TestApp" FILES ${sources_xe_graphics_d3d11_testapp})

add_executable(${target} ${sources_xe_graphics_d3d11_testapp})

target_link_libraries(${target} d3d11)
