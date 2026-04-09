find_package(glfw3 REQUIRED)
find_package(glaze REQUIRED)

set (target xe-glaze-test)
set (sources "main.cpp")

add_executable(${target} ${sources})
target_link_libraries(${target} PRIVATE glfw)
target_link_libraries(${target} PRIVATE glaze::raii glaze::gl glaze::gl_handle)
