
find_package(cgltf CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

set (target xe-gltf-view)
set (sources
        src/xe-gltf-view-main.cpp
        src/GltfMesh.h
        src/GltfProcessor.h
        src/GltfDataLoader.h
        src/GltfDataLoader.cpp
        src/Window.h
        src/Window.cpp
)

add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC cgltf::cgltf PRIVATE SDL2::SDL2 SDL2::SDL2main PUBLIC glad::glad)
