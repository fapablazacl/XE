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
	XE.Core 
	glad-gl4
)

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

if (XE_UNIT_TEST)
	add_subdirectory("unit-test")
endif()
