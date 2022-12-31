#========================================================
# PLUGINS SECTION
#========================================================

# include LodePNG image
if (XE_PLUGIN_PNG)
    add_subdirectory("imageloader-lodepng")
endif ()

if (XE_PLUGIN_DIRECT3D11)
    add_subdirectory ("graphicsdevice-d3d11")
endif ()

# include glfw
if (XE_PLUGIN_GL OR XE_PLUGIN_ES2 OR XE_PLUGIN_VULKAN) 
    find_package(glfw3 REQUIRED)
endif ()

if (XE_PLUGIN_GL)
    add_subdirectory ("dependencies/glad-gl4")
    add_subdirectory ("src/XE.Graphics.GL")
endif ()

if (XE_PLUGIN_GL_GLFW)
    add_subdirectory("src/XE.Graphics.GL.GLFW")
endif()

if (XE_PLUGIN_ES2)
    add_subdirectory ("dependencies/glad-es2")
    add_subdirectory ("src/XE.Graphics.ES2")
endif()

if (XE_PLUGIN_VULKAN)
    add_subdirectory ("src/XE.Graphics.Vk.TestApp")
endif ()

