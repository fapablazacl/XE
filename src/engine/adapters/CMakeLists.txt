if (XE_PLUGIN_PNG)
    add_subdirectory("imageloader-lodepng")
endif ()

if (XE_PLUGIN_DIRECT3D11)
    add_subdirectory ("graphicsdevice-d3d11")
endif ()

if (XE_PLUGIN_GL)
    add_subdirectory ("graphicsdevice-gl")
endif ()

if (XE_PLUGIN_GL_GLFW)
    add_subdirectory("platform-glfw")
endif()

if (XE_PLUGIN_ES2)
    add_subdirectory ("graphicsdevice-gles")
endif()

if (XE_PLUGIN_VULKAN)
    add_subdirectory ("graphicsdevice-vulkan")
endif ()
