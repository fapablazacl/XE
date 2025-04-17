
hunter_add_package(cgltf)
find_package(cgltf CONFIG REQUIRED)

set (target gltf-test)
# add_library(${target} ${sources})
# target_include_directories(${target} PUBLIC "src")

add_executable(${target} src/gltf-test-main.cpp)

target_link_libraries(${target}
    PUBLIC XE.Core
    PRIVATE XE.Interface
        PUBLIC cgltf::cgltf
)

# target_link_libraries(gltf-test XE.GLTF)
