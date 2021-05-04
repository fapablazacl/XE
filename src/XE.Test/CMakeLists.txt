
set (target "XE.Test")

set (src "Main.cpp")

set (src_xe_math 
    "XE/Math/Common.Test.cpp"
    "XE/Math/Vector.Test.cpp"
    "XE/Math/Matrix.Test.cpp"
    "XE/Math/Quaternion.Test.cpp"
)

set (src_xe_scene
    "XE/Scene/Projection.Test.cpp"
    "XE/Scene/Trackball.Test.cpp"
    "XE/Scene/VirtualSphere.Test.cpp"
)

source_group("\\" FILES ${src})
source_group("\\XE\\Math" FILES ${src_xe_math})
source_group("\\XE\\Scene" FILES ${src_xe_scene})

add_executable(${target} 
    ${src}
    ${src_xe_math}
    ${src_xe_scene}
)

target_link_libraries(${target} XE.Core XE.Math XE.Scene)
