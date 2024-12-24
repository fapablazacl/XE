
find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found. vulkan-poc target is unavailable")
    return()
endif()

hunter_add_package(glfw)
find_package(glfw3 CONFIG REQUIRED)

set (target "vulkan-app-poc")
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
