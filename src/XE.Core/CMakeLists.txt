
set (target "XE.Core")

set (src_xe
    "XE/Stream.hpp"
    "XE/Stream.cpp"
    "XE/Buffer.hpp"
    "XE/Buffer.cpp"
    "XE/DataType.hpp"
    "XE/DataType.cpp"
    "XE/DataLayout.cpp"
    "XE/DataLayout.hpp"
	"XE/DataAttribute.cpp"
    "XE/DataAttribute.hpp"
)

set (src_xe_math
    "XE/Math/Vector.hpp"
    "XE/Math/Vector.cpp"
	"XE/Math/Matrix.hpp"
    "XE/Math/Matrix.cpp"
	"XE/Math/Quaternion.hpp"
	"XE/Math/Quaternion.cpp"
	"XE/Math/Boundary.hpp"
	"XE/Math/Boundary.cpp"
	"XE/Math/Box.hpp"
	"XE/Math/Box.cpp"
	"XE/Math/Plane.hpp"
	"XE/Math/Plane.cpp"
	"XE/Math/Ray.hpp"
	"XE/Math/Ray.cpp"
	"XE/Math/Rect.hpp"
	"XE/Math/Rect.cpp"
	"XE/Math/Sphere.hpp"
	"XE/Math/Sphere.cpp"
	"XE/Math/Triangle.hpp"
	"XE/Math/Triangle.cpp"
    "XE/Math/Ellipsoid.hpp"
    "XE/Math/Ellipsoid.cpp"
)

set (src_xe_graphics
	"XE/Graphics/GraphicsDevice.cpp"
	"XE/Graphics/GraphicsDevice.hpp" 
	"XE/Graphics/BufferDescriptor.cpp" 
	"XE/Graphics/BufferDescriptor.hpp" 
	"XE/Graphics/Subset.cpp" 
	"XE/Graphics/Subset.hpp" 
	"XE/Graphics/Texture.cpp" 
	"XE/Graphics/Texture.hpp" 
	"XE/Graphics/Texture2D.cpp" 
	"XE/Graphics/Texture2D.hpp" 
	"XE/Graphics/Texture2DArray.cpp" 
	"XE/Graphics/Texture2DArray.hpp" 
	"XE/Graphics/Texture3D.cpp" 
	"XE/Graphics/Texture3D.hpp" 
	"XE/Graphics/TextureCubeMap.cpp" 
	"XE/Graphics/TextureCubeMap.hpp" 
	"XE/Graphics/PixelFormat.cpp" 
	"XE/Graphics/PixelFormat.hpp"
	"XE/Graphics/Shader.cpp" 
	"XE/Graphics/Shader.hpp"
	"XE/Graphics/Program.cpp" 
	"XE/Graphics/Program.hpp"
	"XE/Graphics/Vertex.cpp"
	"XE/Graphics/Vertex.hpp"
	"XE/Graphics/VertexLayout.cpp"
	"XE/Graphics/VertexLayout.hpp"
	"XE/Graphics/VertexFormat.cpp"
	"XE/Graphics/VertexFormat.hpp"
	"XE/Graphics/UniformLayout.cpp"
	"XE/Graphics/UniformLayout.hpp"
	"XE/Graphics/ImageImpl.hpp"
	"XE/Graphics/ImageImpl.cpp"
	"XE/Graphics/Image.hpp"
	"XE/Graphics/Image.cpp"
	"XE/Graphics/ImageLoader.hpp"
	"XE/Graphics/ImageLoader.cpp"
	"XE/Graphics/Material.hpp"
	"XE/Graphics/Material.cpp"
)

set (src_xe_input
    "XE/Input/InputManager.hpp"
	"XE/Input/InputManager.cpp"
	"XE/Input/DeviceStatus.hpp"
	"XE/Input/DeviceStatus.cpp"
)

set (src_xe_ui
    "XE/UI/Frame.hpp"
	"XE/UI/Frame.cpp"
	"XE/UI/WindowSystem.hpp"
	"XE/UI/WindowSystem.cpp"
)

source_group ("\\XE" FILES ${src_xe})
source_group ("\\XE\\Math" FILES ${src_xe_math})
source_group ("\\XE\\Graphics" FILES ${src_xe_graphics})
source_group ("\\XE\\Input" FILES ${src_xe_input})
source_group ("\\XE\\UI" FILES ${src_xe_ui})

add_library (${target} ${src_xe} ${src_xe_math} ${src_xe_graphics} ${src_xe_input} ${src_xe_ui})
