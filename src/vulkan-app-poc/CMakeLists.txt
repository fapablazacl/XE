
hunter_add_package(glfw)
find_package(glfw3 CONFIG REQUIRED)

find_package(Vulkan 1.2 REQUIRED)

set (target "vulkan-app-poc")
set (src VulkanAppMain.cpp HostPlatform.h HostPlatform.cpp VulkanRenderer.h VulkanRenderer.cpp)

add_executable(${target} ${src})

target_include_directories(${target} PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(
  ${target} 
  PUBLIC XE.Core 
  PUBLIC XE.Math
  PUBLIC glfw
  PRIVATE XE.Interface
  PUBLIC Vulkan::Vulkan
)
