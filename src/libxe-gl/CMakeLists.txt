
hunter_add_package(Microsoft.GSL)
find_package(Microsoft.GSL CONFIG REQUIRED)

hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)

hunter_add_package(glm)
find_package(glm REQUIRED)

add_subdirectory("dependencies/glad-gl4")

set (target "XE.GL")

set (headers
	"src/xe/GL.h"
	"src/xe/gl/gl.h"
	"src/xe/gl/GraphicsDeviceGL.h"
	"src/xe/gl/TextureBaseGL.h"
	"src/xe/gl/Texture2DGL.h"
	"src/xe/gl/Texture2DArrayGL.h"
	"src/xe/gl/Texture3DGL.h"
	"src/xe/gl/TextureCubeMapGL.h"
	"src/xe/gl/ShaderGL.h"
	"src/xe/gl/ProgramGL.h"
	"src/xe/gl/ProgramGL.h"
	"src/xe/gl/BufferGL.h"
	"src/xe/gl/SubsetGL.h"
	"src/xe/gl/Conversion.h"
	"src/xe/gl/UtilGL.h"
	"src/xe/gl/TextureRepository.h"
	"src/xe/gl/Renderer.h"
	"src/xe/gl/RendererGL.h"
)

set (sources
	${headers}
	"src/xe/gl/GraphicsDeviceGL.cpp"
	"src/xe/gl/TextureBaseGL.cpp"
	"src/xe/gl/Texture2DGL.cpp"
	"src/xe/gl/Texture2DArrayGL.cpp"
	"src/xe/gl/Texture3DGL.cpp"
	"src/xe/gl/TextureCubeMapGL.cpp"
	"src/xe/gl/ShaderGL.cpp"
	"src/xe/gl/ProgramGL.cpp"
	"src/xe/gl/ProgramGL.cpp"
	"src/xe/gl/BufferGL.cpp"
	"src/xe/gl/SubsetGL.cpp"
	"src/xe/gl/Conversion.cpp"
	"src/xe/gl/UtilGL.cpp"
	"src/xe/gl/Renderer.cpp"
	"src/xe/gl/TextureRepository.cpp"
	"src/xe/gl/RendererGL.cpp"
) 

add_library (${target} ${sources})

if (XE_DEV_PRECOMPILED_HEADERS)
	target_precompile_headers (${target} PRIVATE ${headers})
endif()

target_include_directories (${target} PUBLIC "src")
target_link_libraries (${target} 
	PUBLIC XE.Core
		PUBLIC libxe-imageloader
	PUBLIC glad-gl4
	PRIVATE XE.Interface
	PUBLIC fmt::fmt
	PUBLIC glm
)

if (XE_DEV_UNIT_TEST)
	add_subdirectory("unit-test")
endif()
