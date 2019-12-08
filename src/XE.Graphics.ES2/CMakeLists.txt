
set (target "XE.Graphics.ES2")
set (source_base "XE/Graphics/ES2")
set (sources_xe_graphics_glcore
	"XE/Graphics/ES2.hpp"

	"${source_base}/IGraphicsContextES2.hpp"
	"${source_base}/IGraphicsContextES2.cpp"
	"${source_base}/GraphicsDeviceES2.hpp"
	"${source_base}/GraphicsDeviceES2.cpp"

	"${source_base}/TextureBaseES2.hpp"
	"${source_base}/TextureBaseES2.cpp"
	"${source_base}/Texture2DES2.hpp"
	"${source_base}/Texture2DES2.cpp"
	"${source_base}/Texture2DArrayES2.hpp"
	"${source_base}/Texture2DArrayES2.cpp"
	"${source_base}/Texture3DES2.hpp"
	"${source_base}/Texture3DES2.cpp"
	"${source_base}/TextureCubeMapES2.hpp"
	"${source_base}/TextureCubeMapES2.cpp"
	
	"${source_base}/ShaderES2.hpp"
	"${source_base}/ShaderES2.cpp"
	"${source_base}/ProgramES2.hpp"
	"${source_base}/ProgramES2.cpp"
	
	"${source_base}/ProgramES2.hpp"
	"${source_base}/ProgramES2.cpp"
	
	"${source_base}/BufferES2.hpp"
	"${source_base}/BufferES2.cpp"
	
	"${source_base}/SubsetES2.hpp"
	"${source_base}/SubsetES2.cpp"

	"${source_base}/Conversion.hpp"
	"${source_base}/Conversion.cpp"

	"${source_base}/Util.hpp"
	"${source_base}/Util.cpp"
) 

source_group ("\\XE\\Graphics\\GL" FILES ${sources_xe_graphics_glcore})

add_library (${target} ${sources_xe_graphics_glcore})

target_link_libraries (${target} glad-es2 XE.Core)
