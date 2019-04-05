
set (target Qt5Rendering)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${target} Qt5Rendering.cpp)

target_link_libraries(${target} PUBLIC
    Qt5::Core
    Qt5::Gui 
    Qt5::Widgets

    XE.Core
    XE.Graphics.GL
)
