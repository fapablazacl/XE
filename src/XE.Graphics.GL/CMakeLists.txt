
set (target "XE.Graphics.GL")

set (sources
	"include/XE/Graphics/GL.hpp"
	"include/XE/Graphics/GL/IGraphicsContextGL.hpp"
	"include/XE/Graphics/GL/GraphicsDeviceGL.hpp"
	"include/XE/Graphics/GL/TextureBaseGL.hpp"
	"include/XE/Graphics/GL/Texture2DGL.hpp"
	"include/XE/Graphics/GL/Texture2DArrayGL.hpp"
	"include/XE/Graphics/GL/Texture3DGL.hpp"
	"include/XE/Graphics/GL/TextureCubeMapGL.hpp"
	"include/XE/Graphics/GL/ShaderGL.hpp"
	"include/XE/Graphics/GL/ProgramGL.hpp"
	"include/XE/Graphics/GL/ProgramGL.hpp"
	"include/XE/Graphics/GL/BufferGL.hpp"
	"include/XE/Graphics/GL/SubsetGL.hpp"
	"include/XE/Graphics/GL/Conversion.hpp"
	"include/XE/Graphics/GL/Util.hpp"

	"src/IGraphicsContextGL.cpp"
	"src/GraphicsDeviceGL.cpp"
	"src/TextureBaseGL.cpp"
	"src/Texture2DGL.cpp"
	"src/Texture2DArrayGL.cpp"
	"src/Texture3DGL.cpp"
	"src/TextureCubeMapGL.cpp"
	"src/ShaderGL.cpp"
	"src/ProgramGL.cpp"
	"src/ProgramGL.cpp"
	"src/BufferGL.cpp"
	"src/SubsetGL.cpp"
	"src/Conversion.cpp"
	"src/Util.cpp"	
) 

add_library (${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries (${target} glad-gl4 XE.Core XE.Graphics)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
