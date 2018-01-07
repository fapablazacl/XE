
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
)

source_group("\\XE" FILES ${src_xe})
source_group("\\XE\\Math" FILES ${src_xe_math})

add_library(${target} ${src_xe} ${src_xe_math})
