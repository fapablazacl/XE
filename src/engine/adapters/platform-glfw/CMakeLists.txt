set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)

find_package(glfw3 REQUIRED)

set (target XE.Platform.GLFW)

set (headers
    "src/xe/platform/glfw/WindowGLFW.h"
    "src/xe/platform/glfw/GraphicsContextGLFW.h"
    "src/xe/platform/glfw/InputManagerGLFW.h"
)

set (sources
    ${headers}
    "src/xe/platform/glfw/WindowGLFW.cpp"
    "src/xe/platform/glfw/GraphicsContextGLFW.cpp"
    "src/xe/platform/glfw/InputManagerGLFW.cpp"    
)

add_library(${target} ${sources})

if (XE_DEV_PRECOMPILED_HEADERS)
    target_precompile_headers(${target} PRIVATE ${headers})
endif()

target_include_directories(${target} PUBLIC "src/")
target_link_libraries(${target} 
    PUBLIC glfw 
    PUBLIC XE.Core 
    PRIVATE XE.Interface
)
