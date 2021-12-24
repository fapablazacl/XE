
find_package (Vulkan REQUIRED)

include_directories (${Vulkan_INCLUDE_DIRS})

set (target "XE.Graphics.Vulkan.TestApp")
set (src "Main.cpp")

add_executable(${target} ${src})

target_link_libraries(${target} XE.Core ${Vulkan_LIBRARIES} glfw )
