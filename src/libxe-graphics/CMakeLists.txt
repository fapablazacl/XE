
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(Backport CONFIG REQUIRED)

set (target "libxe-graphics")

set (headers
	# xe/graphics/ — abstraction layer (originally libxe-core/src/xe/graphics/)
	"src/xe/graphics/Graphics.h"
	"src/xe/graphics/GraphicsAPI.h"
	"src/xe/graphics/GraphicsContext.h"
	"src/xe/graphics/GraphicsDevice.h"
	"src/xe/graphics/GraphicsDeviceFactory.h"
	"src/xe/graphics/GraphicsManager.h"
	"src/xe/graphics/IWindow.h"
	"src/xe/graphics/BufferDescriptor.h"
	"src/xe/graphics/Subset.h"
	"src/xe/graphics/Texture.h"
	"src/xe/graphics/Texture2D.h"
	"src/xe/graphics/Texture2DArray.h"
	"src/xe/graphics/Texture3D.h"
	"src/xe/graphics/TextureCubeMap.h"
	"src/xe/graphics/PixelFormat.h"
	"src/xe/graphics/Shader.h"
	"src/xe/graphics/Program.h"
	"src/xe/graphics/Vertex.h"
	"src/xe/graphics/VertexLayout.h"
	"src/xe/graphics/VertexFormat.h"
	"src/xe/graphics/Uniform.h"
	"src/xe/graphics/ImageImpl.h"
	"src/xe/graphics/Image.h"
	"src/xe/graphics/ImageLoader.h"
	"src/xe/graphics/Material.h"
	"src/xe/graphics/Viewport.h"

	# xe/gl/ — OpenGL 3 Core Profile reference implementation (originally libxe-gl/)
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
	"src/xe/gl/BufferGL.h"
	"src/xe/gl/SubsetGL.h"
	"src/xe/gl/Conversion.h"
	"src/xe/gl/UtilGL.h"
	"src/xe/gl/TextureRepository.h"
	"src/xe/gl/Renderer.h"
	"src/xe/gl/RendererGL.h"
	"src/xe/gl/Types.h"

	# xe/render/ — new vtable+handle backend prototype (originally xe-glaze-test/src/xe/render/)
	"src/xe/render/RenderBackend.h"
	"src/xe/render/types.h"
	"src/xe/render/backend/glcore3-api.h"
)

set (sources
	${headers}

	# xe/graphics/ implementations
	"src/xe/graphics/GraphicsContext.cpp"
	"src/xe/graphics/GraphicsDevice.cpp"
	"src/xe/graphics/GraphicsDeviceFactory.cpp"
	"src/xe/graphics/GraphicsManager.cpp"
	"src/xe/graphics/IWindow.cpp"
	"src/xe/graphics/Subset.cpp"
	"src/xe/graphics/Texture.cpp"
	"src/xe/graphics/Texture2D.cpp"
	"src/xe/graphics/Texture2DArray.cpp"
	"src/xe/graphics/Texture3D.cpp"
	"src/xe/graphics/TextureCubeMap.cpp"
	"src/xe/graphics/PixelFormat.cpp"
	"src/xe/graphics/Shader.cpp"
	"src/xe/graphics/Program.cpp"
	"src/xe/graphics/ImageImpl.cpp"
	"src/xe/graphics/Image.cpp"
	"src/xe/graphics/ImageLoader.cpp"
	"src/xe/graphics/Material.cpp"

	# xe/gl/ implementations
	"src/xe/gl/GraphicsDeviceGL.cpp"
	"src/xe/gl/TextureBaseGL.cpp"
	"src/xe/gl/Texture2DGL.cpp"
	"src/xe/gl/Texture2DArrayGL.cpp"
	"src/xe/gl/Texture3DGL.cpp"
	"src/xe/gl/TextureCubeMapGL.cpp"
	"src/xe/gl/ShaderGL.cpp"
	"src/xe/gl/ProgramGL.cpp"
	"src/xe/gl/BufferGL.cpp"
	"src/xe/gl/SubsetGL.cpp"
	"src/xe/gl/Conversion.cpp"
	"src/xe/gl/UtilGL.cpp"
	"src/xe/gl/Renderer.cpp"
	"src/xe/gl/TextureRepository.cpp"
	"src/xe/gl/RendererGL.cpp"

	# xe/render/ implementations
	"src/xe/render/RenderBackend.cpp"
	"src/xe/render/backend/glcore3-api.cpp"
)

add_library (${target} ${sources})
add_library(xe::graphics ALIAS ${target})

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
	PUBLIC Backport::Backport
	PUBLIC Microsoft.GSL::Microsoft.GSL
)

if (XE_DEV_UNIT_TEST)
	add_subdirectory("unit-test")
endif()
