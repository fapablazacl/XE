
find_package(glfw3 CONFIG REQUIRED)
find_package (Vulkan REQUIRED)

set (target "XE.Graphics.Vulkan.TestApp")
set (src VulkanAppMain.cpp HostPlatform.h HostPlatform.cpp VulkanRenderer.h VulkanRenderer.cpp)

add_executable(${target} ${src})

target_include_directories(${target} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(
  ${target} 
  PUBLIC XE.Core 
  PUBLIC XE.Math
  PUBLIC ${Vulkan_LIBRARIES} 
  PUBLIC glfw
  PRIVATE XE.Interface
)
