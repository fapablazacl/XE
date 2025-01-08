
find_package(Assimp)
if (NOT Assimp_FOUND)
    MESSAGE(WARNING "Host Assimp not found. Managing it via Hunter")
    hunter_add_package(Assimp)
    find_package(Assimp REQUIRED)
endif()

hunter_add_package(glfw)
find_package(glfw3 REQUIRED)

hunter_add_package(nlohmann_json)
find_package(nlohmann_json REQUIRED)

find_package(FreeImage REQUIRED)

set (target apostate)
add_executable(${target}
    "MiscUtils.h"

    "AssetPackage.h"
    "Image.h"
    "ImageLoader.h"
    "Model.h"
    "ModelLoader.h"

    "Platform.h"
    "AssetPackage.cpp"
    "Model.cpp"
    "ImageLoader.cpp"

    "Platform.cpp"

    # FreeImage
    "ImageLoaderFI.h"
    "ImageLoaderFI.cpp"

    # AssImp
    "ModelLoaderAssimp.h"
    "ModelLoaderAssimp.cpp"
    Logger.cpp
    Logger.h

    ApostateMain.cpp
)

target_link_libraries(
    ${target}
    assimp::assimp
    glfw
    nlohmann_json::nlohmann_json
    freeimage::FreeImage
        XE.GL
)
