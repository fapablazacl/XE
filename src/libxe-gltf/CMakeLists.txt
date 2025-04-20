
hunter_add_package(cgltf)
find_package(cgltf CONFIG REQUIRED)

hunter_add_package(glfw)
find_package(glfw3 REQUIRED)

set (target gltf-test)

add_executable(${target} src/gltf-test-main.cpp)
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC cgltf::cgltf PRIVATE glfw)
