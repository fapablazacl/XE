
find_package(cgltf CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Assimp CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)
find_package(DevIL CONFIG REQUIRED)

set (target xe-gltfc)
set (sources
        src/xe-gltfc-main.cpp
)

add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PRIVATE XE.Interface PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC cgltf::cgltf PUBLIC glad::glad PUBLIC imgui::imgui PUBLIC assimp::assimp PUBLIC DevIL::IL PUBLIC KTX::ktx)
