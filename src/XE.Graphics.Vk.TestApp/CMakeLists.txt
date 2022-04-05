
find_package (Vulkan REQUIRED)

set (target "XE.Graphics.Vulkan.TestApp")
set (src "Main.cpp")

add_executable(${target} ${src})

target_include_directories(${target} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${target} XE.Core ${Vulkan_LIBRARIES} glfw )

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${target} PRIVATE -Wall -Wextra)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Werror)
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${target} PRIVATE /Wall)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE /WX)
    endif()
endif()
