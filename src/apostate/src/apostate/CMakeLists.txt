
find_package(Assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(FreeImage REQUIRED)

set (target apostate)
add_executable(${target}
    "MiscUtils.h"

    "AssetPackage.h"
    "Model.h"
    "ModelLoader.h"

    "Platform.h"
    "AssetPackage.cpp"
    "Model.cpp"

    "Platform.cpp"

    # AssImp
    "ModelLoaderAssimp.h"
    "ModelLoaderAssimp.cpp"

    ApostateMain.cpp
)

target_link_libraries(
    ${target}
    assimp::assimp
    glfw
    nlohmann_json::nlohmann_json
    freeimage::FreeImage
        XE.GL
        libxe-imageloader
)
