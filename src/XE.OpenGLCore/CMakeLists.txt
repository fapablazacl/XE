
set (target "XE.OpenGLCore")
set (source_base "XE/Graphics/GL")
set (sources_xe_graphics_glcore
	"${source_base}/GraphicsDeviceGL.hpp"
	"${source_base}/GraphicsDeviceGL.cpp"
	
	"${source_base}/Texture2DGL.hpp"
	"${source_base}/Texture2DGL.cpp"
	"${source_base}/Texture2DArrayGL.hpp"
	"${source_base}/Texture2DArrayGL.cpp"
	"${source_base}/Texture3DGL.hpp"
	"${source_base}/Texture3DGL.cpp"
	"${source_base}/TextureCubeMapGL.hpp"
	"${source_base}/TextureCubeMapGL.cpp"
	
	"${source_base}/ShaderGL.hpp"
	"${source_base}/ShaderGL.cpp"
	"${source_base}/ProgramGL.hpp"
	"${source_base}/ProgramGL.cpp"
	
	"${source_base}/ProgramGL.hpp"
	"${source_base}/ProgramGL.cpp"
	
	"${source_base}/BufferGL.hpp"
	"${source_base}/BufferGL.cpp"
	
	"${source_base}/MeshSubsetGL.hpp"
	"${source_base}/MeshSubsetGL.cpp"
) 

source_group("\\XE\\Graphics\\GL" FILES ${sources_xe_graphics_glcore})

add_library(${target} ${sources_xe_graphics_glcore})
