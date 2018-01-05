
set (target "XE.Core")

set (src_xe_math
    "XE/Math/Vector.hpp"
    "XE/Math/Vector.cpp"
	"XE/Math/Matrix.hpp"
    "XE/Math/Matrix.cpp"
)

source_group("\\XE\\Math" FILES ${src_xe_math})

add_library(${target} ${src_xe_math})
