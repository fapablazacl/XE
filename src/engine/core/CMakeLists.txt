
set (target "XE.Core")

set (headers 
	"src/xe/XE.h"
	"src/xe/Predef.h"
	"src/xe/Buffer.h"
	"src/xe/DataType.h"
	"src/xe/Core.h"
	"src/xe/Timer.h"

	"src/xe/graphics/Graphics.h"
	"src/xe/graphics/IWindow.h"
	"src/xe/graphics/GraphicsAPI.h" 
	"src/xe/graphics/GraphicsContext.h" 
	"src/xe/graphics/GraphicsDeviceFactory.h" 
	"src/xe/graphics/GraphicsDevice.h" 
	"src/xe/graphics/GraphicsManager.h" 
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

    "src/xe/input/Input.h"
    "src/xe/input/InputManager.h"
    "src/xe/input/DeviceStatus.h"

	"src/xe/io/FileStream.h"
    "src/xe/io/FileStreamSource.h"
    "src/xe/io/MemoryStream.h"
    "src/xe/io/Stream.h"
    "src/xe/io/StreamSource.h"

	"src/xe/math/Math.h"
    "src/xe/math/Common.h"
    "src/xe/math/Vector.h"
    "src/xe/math/Matrix.h"
    "src/xe/math/Quaternion.h"
    "src/xe/math/Boundary.h"
    "src/xe/math/Box.h"
    "src/xe/math/Plane.h"
    "src/xe/math/Ray.h"
    "src/xe/math/Rect.h"
    "src/xe/math/Sphere.h"
    "src/xe/math/Triangle.h"
    "src/xe/math/Ellipsoid.h"
)

set (sources
	${headers}

	"src/xe/Buffer.cpp"
    "src/xe/DataType.cpp"
	"src/xe/Core.cpp"
	"src/xe/Timer.cpp"

	"src/xe/graphics/IWindow.cpp"	
	"src/xe/graphics/GraphicsDevice.cpp"
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

	"src/xe/input/DeviceStatus.cpp"
    "src/xe/input/InputManager.cpp"
	
	"src/xe/io/FileStream.cpp"
	"src/xe/io/FileStreamSource.cpp"
	"src/xe/io/MemoryStream.cpp"
	"src/xe/io/Stream.cpp"
	"src/xe/io/StreamSource.cpp"
        src/xe/math/Quaternion.cpp src/xe/math/Vector.cpp)

add_library (${target} ${sources})

if (XE_DEV_PRECOMPILED_HEADERS)
	target_precompile_headers(${target} PRIVATE ${headers})
endif()

target_include_directories(${target} PUBLIC "src/")
target_link_libraries(${target} PRIVATE XE.Interface)

if (XE_DEV_UNIT_TEST)
	add_subdirectory("test/math")
endif()
