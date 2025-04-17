
hunter_add_package(cgltf)
find_package(cgltf CONFIG REQUIRED)

set (target XE.GLTF)

set (sources
    src/Asset_CGLTF.h
    src/Asset_CGLTF.cpp
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "src")
target_link_libraries(${target}
    PUBLIC XE.Core
    PRIVATE XE.Interface
        PUBLIC cgltf::cgltf
)


add_executable(gltf-test src/gltf-test-main.cpp)
target_link_libraries(gltf-test XE.GLTF)
