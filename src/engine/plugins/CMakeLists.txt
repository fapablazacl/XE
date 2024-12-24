if (XE_PLUGIN_PNG)
    add_subdirectory("libxe-core-imageloader-lodepng")
endif ()

if (XE_PLUGIN_DIRECT3D11)
    add_subdirectory ("d3d11-poc")
endif ()

if (XE_PLUGIN_GL)
    add_subdirectory ("libxe-core-graphicsdevice-gl")
endif ()

if (XE_PLUGIN_GL_GLFW)
    add_subdirectory("libxe-core-platform-glfw")
endif()

if (XE_PLUGIN_ES2)
    add_subdirectory ("libxe-core-graphicsdevice-gles")
endif()

if (XE_PLUGIN_VULKAN)
    add_subdirectory ("vulkan-poc")
endif ()
