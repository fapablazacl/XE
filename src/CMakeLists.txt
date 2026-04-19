# add_subdirectory("vulkan-app-poc")
# add_subdirectory("d3d11-app-poc")

add_subdirectory ("libxe-core")

add_subdirectory ("libxe-math")
if (XE_DEV_UNIT_TEST)
    add_subdirectory ("libxe-math-test")
endif()

add_subdirectory ("libxe-scene")
add_subdirectory ("libxe-geometry")
# add_subdirectory ("libxe-app")

# DevIL doesn't build in MSVC
# add_subdirectory ("libxe-imageloader")

# add_subdirectory ("xe-gltf-view")
# add_subdirectory ("xe-gltfc")

if (XE_PLUGIN_KTX)
    # add_subdirectory ("xe-ktxc")
endif()

if (XE_PLUGIN_PNG)
    # add_subdirectory("libxe-core-imageloader-lodepng")
endif ()

add_subdirectory ("libxe-graphics")

if (XE_PLUGIN_GL_GLFW)
    # add_subdirectory("libxe-core-platform-glfw")
endif()

# add_subdirectory("apostate")
# add_subdirectory("capybaria")
add_subdirectory("xe-glaze-test")
