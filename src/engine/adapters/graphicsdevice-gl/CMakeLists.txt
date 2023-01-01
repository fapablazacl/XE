add_subdirectory("dependencies/glad-gl4")

set (target "XE.Graphics.GL")

set (headers
	"include/xe/graphics/GL.h"
	"include/xe/graphics/gl/glcore.h"
	"include/xe/graphics/gl/GraphicsDeviceGL.h"
	"include/xe/graphics/gl/TextureBaseGL.h"
	"include/xe/graphics/gl/Texture2DGL.h"
	"include/xe/graphics/gl/Texture2DArrayGL.h"
	"include/xe/graphics/gl/Texture3DGL.h"
	"include/xe/graphics/gl/TextureCubeMapGL.h"
	"include/xe/graphics/gl/ShaderGL.h"
	"include/xe/graphics/gl/ProgramGL.h"
	"include/xe/graphics/gl/ProgramGL.h"
	"include/xe/graphics/gl/BufferGL.h"
	"include/xe/graphics/gl/SubsetGL.h"
	"include/xe/graphics/gl/Conversion.h"
	"include/xe/graphics/gl/Util.h"
)

set (sources
	${headers}
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

target_precompile_headers (${target} PRIVATE ${headers})
target_include_directories (${target} PUBLIC "include/")
target_link_libraries (${target} 
	PUBLIC XE.Core 
	PUBLIC glad-gl4
	PRIVATE XE.Interface
)

if (XE_DEV_UNIT_TEST)
	add_subdirectory("unit-test")
endif()
