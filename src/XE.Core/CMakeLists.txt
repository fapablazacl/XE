
set (target "XE.Core")

set (src_xe
	"XE/Predef.hpp"
    "XE/Buffer.hpp"
    "XE/Buffer.cpp"
    "XE/DataType.hpp"
    "XE/DataType.cpp"
	"XE/Core.hpp"
	"XE/Core.cpp"

	# these will be future C++20 Modules
	"XE.hpp"
	"XE/Graphics.hpp"
	"XE/Math.hpp"
	"XE/Input.hpp"
)

set (src_xe_io
	"XE/IO/FileStream.hpp"
	"XE/IO/FileStream.cpp"
	"XE/IO/FileStreamSource.hpp"
	"XE/IO/FileStreamSource.cpp"
	"XE/IO/MemoryStream.hpp"
	"XE/IO/MemoryStream.cpp"
	"XE/IO/Stream.hpp"
	"XE/IO/Stream.cpp"
	"XE/IO/StreamSource.hpp"
	"XE/IO/StreamSource.cpp"
)

set (src_xe_math
	"XE/Math/Common.hpp"
    "XE/Math/Common.cpp"
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

set (src_xe_scene
	"XE/Scene/Projection.hpp"
	"XE/Scene/Projection.cpp"
	"XE/Scene/Trackball.hpp"
	"XE/Scene/Trackball.cpp"
	"XE/Scene/VirtualSphere.hpp"
	"XE/Scene/VirtualSphere.cpp"
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

set (src_xe_util
	"XE/Util/Timer.hpp"
	"XE/Util/Timer.cpp"
)


source_group ("\\XE" FILES ${src_xe})
source_group ("\\XE\\Math" FILES ${src_xe_math})
source_group ("\\XE\\IO" FILES ${src_xe_io})
source_group ("\\XE\\Scene" FILES ${src_xe_scene})
source_group ("\\XE\\Input" FILES ${src_xe_input})
source_group ("\\XE\\UI" FILES ${src_xe_ui})
source_group ("\\XE\\Util" FILES ${src_xe_util})

add_library (${target} 
	${src_xe} 
	${src_xe_math} 
	${src_xe_io} 
	${src_xe_scene}
	${src_xe_input} 
	${src_xe_ui}
	${src_xe_util}
)

#
#if (UNIX)
#	target_link_libraries(${target} stdc++fs)
#endif()
