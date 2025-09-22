add_subdirectory("vulkan-app-poc")
add_subdirectory("d3d11-app-poc")

add_subdirectory ("libxe-core")
add_subdirectory ("libxe-math")
add_subdirectory ("libxe-scene")
add_subdirectory ("libxe-geometry")
add_subdirectory ("libxe-app")
add_subdirectory ("libxe-imageloader")

add_subdirectory ("xe-gltf-view")
add_subdirectory ("xe-gltfc")
add_subdirectory ("xe-ktxc")

if (XE_PLUGIN_PNG)
    add_subdirectory("libxe-core-imageloader-lodepng")
endif ()

if (XE_PLUGIN_GL)
    add_subdirectory ("libxe-gl")
endif ()

if (XE_PLUGIN_GL_GLFW)
    add_subdirectory("libxe-core-platform-glfw")
endif()

add_subdirectory("apostate")
add_subdirectory("capybaria")
