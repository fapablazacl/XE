
find_package (Vulkan REQUIRED)

set (target "XE.Graphics.Vulkan.TestApp")
set (src "Main.cpp")

add_executable(${target} ${src})

target_include_directories(${target} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(
  ${target} 
  PUBLIC XE.Core 
  PUBLIC ${Vulkan_LIBRARIES} 
  PUBLIC glfw 
  PRIVATE XE.Interface
)
