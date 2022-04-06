
set (target "XE.Graphics")

set (headers 
	"include/XE/Graphics.h"
	"include/XE/Graphics/IWindow.h"
	"include/XE/Graphics/GraphicsAPI.h" 
	"include/XE/Graphics/GraphicsContext.h" 
	"include/XE/Graphics/GraphicsDeviceFactory.h" 
	"include/XE/Graphics/GraphicsDevice.h" 
	"include/XE/Graphics/GraphicsManager.h" 
	"include/XE/Graphics/BufferDescriptor.h" 
	"include/XE/Graphics/Subset.h" 
	"include/XE/Graphics/Texture.h" 
	"include/XE/Graphics/Texture2D.h" 
	"include/XE/Graphics/Texture2DArray.h" 
	"include/XE/Graphics/Texture3D.h" 
	"include/XE/Graphics/TextureCubeMap.h" 
	"include/XE/Graphics/PixelFormat.h"
	"include/XE/Graphics/Shader.h"
	"include/XE/Graphics/Program.h"
	"include/XE/Graphics/Vertex.h"
	"include/XE/Graphics/VertexLayout.h"
	"include/XE/Graphics/VertexFormat.h"
	"include/XE/Graphics/Uniform.h"
	"include/XE/Graphics/ImageImpl.h"
	"include/XE/Graphics/Image.h"
	"include/XE/Graphics/ImageLoader.h"
	"include/XE/Graphics/Material.h"
	"include/XE/Graphics/Viewport.h"
)

set (sources
	${headers}

	"src/IWindow.cpp"	
	"src/GraphicsDevice.cpp"
	"src/Subset.cpp" 
	"src/Texture.cpp" 
	"src/Texture2D.cpp" 
	"src/Texture2DArray.cpp" 
	"src/Texture3D.cpp" 
	"src/TextureCubeMap.cpp" 
	"src/PixelFormat.cpp" 
	"src/Shader.cpp" 
	"src/Program.cpp" 
	"src/ImageImpl.cpp"
	"src/Image.cpp"
	"src/ImageLoader.cpp"
	"src/Material.cpp"
)

add_library (${target} ${sources})

target_precompile_headers (${target} PRIVATE ${headers})
target_include_directories (${target} PUBLIC "include/")
target_link_libraries (${target} XE.Core XE.Math)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${target} PRIVATE -Wall -Wextra)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Werror)
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_compile_options(${target} PRIVATE /Wall)

	if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE /WX)
	endif()
endif()

if (XE_EXAMPLE_DEMO)
	add_subdirectory(demo)
endif ()
