
set (target "XE.Graphics.GL")

set (sources
	"include/XE/Graphics/GL.h"
	"include/XE/Graphics/GL/IGraphicsContextGL.h"
	"include/XE/Graphics/GL/GraphicsDeviceGL.h"
	"include/XE/Graphics/GL/TextureBaseGL.h"
	"include/XE/Graphics/GL/Texture2DGL.h"
	"include/XE/Graphics/GL/Texture2DArrayGL.h"
	"include/XE/Graphics/GL/Texture3DGL.h"
	"include/XE/Graphics/GL/TextureCubeMapGL.h"
	"include/XE/Graphics/GL/ShaderGL.h"
	"include/XE/Graphics/GL/ProgramGL.h"
	"include/XE/Graphics/GL/ProgramGL.h"
	"include/XE/Graphics/GL/BufferGL.h"
	"include/XE/Graphics/GL/SubsetGL.h"
	"include/XE/Graphics/GL/Conversion.h"
	"include/XE/Graphics/GL/Util.h"

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
