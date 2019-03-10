
set (target "XE.Test")

set (src "Main.cpp")
set (src_xe_math 
    "XE/Math/Common.Test.cpp"
    "XE/Math/Vector.Test.cpp"
    "XE/Scene/Projection.Test.cpp"
)

source_group("\\" FILES ${src})
source_group("\\XE\\Math" FILES ${src_xe_math})

add_executable(${target} 
    ${src}
    ${src_xe_math}
)

target_link_libraries(${target} XE.Core)
