
hunter_add_package(cgltf)
find_package(cgltf CONFIG REQUIRED)

hunter_add_package(glfw)
find_package(glfw3 REQUIRED)

set (target gltf-test)
set (sources
        src/gltf-test-main.cpp
        include/GltfProcessor.h
        include/GltfLoader.h
        src/GltfLoader.cpp
)

add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC cgltf::cgltf PRIVATE glfw)
