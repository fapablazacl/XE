
add_subdirectory("dependencies/glad-es2")

set (target "XE.Graphics.ES2")

set (headers
	"include/XE/Graphics/ES2.h"
	"include/xe/graphics/gles2/GraphicsDeviceES2.h"
	"include/xe/graphics/gles2/TextureBaseES2.h"
	"include/xe/graphics/gles2/Texture2DES2.h"
	"include/xe/graphics/gles2/TextureCubeMapES2.h"
	"include/xe/graphics/gles2/ShaderES2.h"
	"include/xe/graphics/gles2/ProgramES2.h"
	"include/xe/graphics/gles2/BufferES2.h"
	"include/xe/graphics/gles2/SubsetES2.h"
	"include/xe/graphics/gles2/ConversionES.h"
	"include/xe/graphics/gles2/Util.h"
)

set (sources
	${headers}
	"src/GraphicsDeviceES2.cpp"
	"src/TextureBaseES2.cpp"
	"src/Texture2DES2.cpp"
	"src/TextureCubeMapES2.cpp"
	"src/ShaderES2.cpp"
	"src/ProgramES2.cpp"
	"src/ProgramES2.cpp"
	"src/BufferES2.cpp"
	"src/SubsetES2.cpp"
	"src/ConversionES.cpp"
	"src/Util.cpp"
) 

add_library (${target} ${sources})

target_precompile_headers (${target} PRIVATE ${headers})
target_include_directories (${target} PUBLIC "include/")
target_link_libraries (${target} 
	XE.Core 
	XE.Graphics
	glad-es2
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
