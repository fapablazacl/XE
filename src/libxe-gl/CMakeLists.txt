
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)

set (target "xe-gl")

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
add_library(xe::gl ALIAS ${target})

if (XE_DEV_PRECOMPILED_HEADERS)
	target_precompile_headers (${target} PRIVATE ${headers})
endif()

target_include_directories (${target} PUBLIC "src")
target_link_libraries (${target}
	PUBLIC xe::core
	PUBLIC xe::imageloader
	PUBLIC glaze::gl
	PRIVATE xe::interface
	PUBLIC fmt::fmt
	PUBLIC glm::glm
	PUBLIC glfw
)

if (XE_DEV_UNIT_TEST)
	add_subdirectory("unit-test")
endif()
