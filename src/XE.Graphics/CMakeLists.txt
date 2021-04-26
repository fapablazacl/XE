
set (target "XE.Graphics")

set (sources
	"include/XE/Graphics/IWindow.hpp"
	"include/XE/Graphics/GraphicsDevice.hpp" 
	"include/XE/Graphics/BufferDescriptor.hpp" 
	"include/XE/Graphics/Subset.hpp" 
	"include/XE/Graphics/Texture.hpp" 
	"include/XE/Graphics/Texture2D.hpp" 
	"include/XE/Graphics/Texture2DArray.hpp" 
	"include/XE/Graphics/Texture3D.hpp" 
	"include/XE/Graphics/TextureCubeMap.hpp" 
	"include/XE/Graphics/PixelFormat.hpp"
	"include/XE/Graphics/Shader.hpp"
	"include/XE/Graphics/Program.hpp"
	"include/XE/Graphics/Vertex.hpp"
	"include/XE/Graphics/VertexLayout.hpp"
	"include/XE/Graphics/VertexFormat.hpp"
	"include/XE/Graphics/Uniform.hpp"
	"include/XE/Graphics/ImageImpl.hpp"
	"include/XE/Graphics/Image.hpp"
	"include/XE/Graphics/ImageLoader.hpp"
	"include/XE/Graphics/Material.hpp"
	"include/XE/Graphics/Viewport.hpp"

	"src/IWindow.cpp"	
	"src/GraphicsDevice.cpp"
	"src/BufferDescriptor.cpp" 
	"src/Subset.cpp" 
	"src/Texture.cpp" 
	"src/Texture2D.cpp" 
	"src/Texture2DArray.cpp" 
	"src/Texture3D.cpp" 
	"src/TextureCubeMap.cpp" 
	"src/PixelFormat.cpp" 
	"src/Shader.cpp" 
	"src/Program.cpp" 
	"src/Vertex.cpp"
	"src/VertexLayout.cpp"
	"src/VertexFormat.cpp"
	"src/Uniform.cpp"
	"src/ImageImpl.cpp"
	"src/Image.cpp"
	"src/ImageLoader.cpp"
	"src/Material.cpp"
	"src/Viewport.cpp"
)

add_library (${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core XE.Math)
target_compile_options (${target} PUBLIC ${XE_CXX_FLAGS})
