
find_package(cgltf CONFIG REQUIRED)
find_package(glfw3 REQUIRED)

set (target xe-gltf-view)
set (sources
        src/gltf-test-main.cpp
        src/GltfProcessor.h
        src/GltfDataLoader.h
        src/GltfDataLoader.cpp
)

add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC cgltf::cgltf PRIVATE glfw)
