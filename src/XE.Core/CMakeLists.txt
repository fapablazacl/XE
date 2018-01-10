
set (target "XE.Core")

set (src_xe
    "XE/Stream.hpp"
    "XE/Stream.cpp"
    "XE/Buffer.hpp"
    "XE/Buffer.cpp"
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
)

set (src_xe_graphics
	"XE/Graphics/GraphicsDevice.cpp"
	"XE/Graphics/GraphicsDevice.hpp" 
	"XE/Graphics/Subset.cpp" 
	"XE/Graphics/Subset.hpp" 
	"XE/Graphics/Texture.cpp" 
	"XE/Graphics/Texture.hpp" 
	"XE/Graphics/PixelFormat.cpp" 
	"XE/Graphics/PixelFormat.hpp" 
)

source_group("\\XE" FILES ${src_xe})
source_group("\\XE\\Math" FILES ${src_xe_math})
source_group("\\XE\\Graphics" FILES ${src_xe_graphics})

add_library(${target} ${src_xe} ${src_xe_math} ${src_xe_graphics})
