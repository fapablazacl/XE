
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
source_group ("\\XE\\UI" FILES ${src_xe_ui})
source_group ("\\XE\\Util" FILES ${src_xe_util})

add_library (${target} 
	${src_xe} 
	${src_xe_ui}
	${src_xe_util}
)

#if (UNIX)
#	target_link_libraries(${target} stdc++fs)
#endif()
