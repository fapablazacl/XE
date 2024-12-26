
find_package(Assimp)
if (NOT Assimp_FOUND)
    MESSAGE(WARNING "Host Assimp not found. Managing it via Hunter")
    hunter_add_package(Assimp)
    find_package(Assimp REQUIRED)
endif()

hunter_add_package(glfw)
find_package(glfw3 REQUIRED)

hunter_add_package(glm)
find_package(glm REQUIRED)

hunter_add_package(nlohmann_json)
find_package(nlohmann_json REQUIRED)

hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)

find_package(FreeImage REQUIRED)

set (target apostate)
add_executable(${target}
    "MiscUtils.h"

        "AssetPackage.h"
    "Image.h"
    "ImageLoader.h"
    "Model.h"
    "ModelLoader.h"
    "Renderer.h"
    "Platform.h"
    "TextureRepository.h"

        "AssetPackage.cpp"
    "Model.cpp"
    "ImageLoader.cpp"
    "Renderer.cpp"
    "Platform.cpp"
    "TextureRepository.cpp"

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
        glm
        glad
        nlohmann_json::nlohmann_json
        freeimage::FreeImage
        fmt::fmt
)
