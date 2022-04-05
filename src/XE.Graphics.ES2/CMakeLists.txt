
set (target "XE.Graphics.ES2")

set (headers
	"include/XE/Graphics/ES2.h"
	"include/XE/Graphics/ES2/GraphicsDeviceES2.h"
	"include/XE/Graphics/ES2/TextureBaseES2.h"
	"include/XE/Graphics/ES2/Texture2DES2.h"
	"include/XE/Graphics/ES2/TextureCubeMapES2.h"
	"include/XE/Graphics/ES2/ShaderES2.h"
	"include/XE/Graphics/ES2/ProgramES2.h"
	"include/XE/Graphics/ES2/BufferES2.h"
	"include/XE/Graphics/ES2/SubsetES2.h"
	"include/XE/Graphics/ES2/ConversionES.h"
	"include/XE/Graphics/ES2/Util.h"
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
