
find_package(glfw3 REQUIRED)
find_package(glaze REQUIRED)

set (target xe-glaze-test)
set (sources src/xe-glaze-test.cpp)
add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC src)
target_link_libraries(${target} PRIVATE glfw)
target_link_libraries(${target} PRIVATE glaze::raii glaze::gl glaze::gl_compat glaze::gles2 xe::math)

set (target_renderer xe-render-core)
set (sources_renderer
    src/xe-render-core-test.cpp
    src/xe/render/RenderBackend.h 
    src/xe/render/RenderBackend.cpp 
    src/xe/render/types.h 
    src/xe/render/backend/glcore3-api.h
    src/xe/render/backend/glcore3-api.cpp
)
add_executable(${target_renderer} ${sources_renderer})
target_include_directories(${target_renderer} PUBLIC src)
target_link_libraries(${target_renderer} PRIVATE glfw)
target_link_libraries(${target_renderer} PRIVATE glaze::raii glaze::gl glaze::gl_compat glaze::gles2 xe::math xe::core)
