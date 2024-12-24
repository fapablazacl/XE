
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

find_package(FreeImage REQUIRED)

set (target apostate-core)
add_library(${target} 
    "src/apostate/MiscUtils.h"

    "src/apostate/Scenario.h"
    "src/apostate/ScenarioImporter.h"
    "src/apostate/ScenarioParser.h"
    "src/apostate/AssetPackage.h"
    "src/apostate/Image.h"
    "src/apostate/ImageLoader.h"
    "src/apostate/Model.h"
    "src/apostate/ModelLoader.h"
    "src/apostate/Renderer.h"
    "src/apostate/Platform.h"
    "src/apostate/TextureRepository.h"
    
    "src/apostate/Scenario.cpp"
    "src/apostate/ScenarioImporter.cpp"
    "src/apostate/ScenarioParser.cpp"
    "src/apostate/AssetPackage.cpp"
    "src/apostate/Model.cpp"
    "src/apostate/ImageLoader.cpp"
    "src/apostate/Renderer.cpp"
    "src/apostate/Platform.cpp"
    "src/apostate/TextureRepository.cpp"

    # FreeImage
    "src/apostate/ImageLoaderFI.h"
    "src/apostate/ImageLoaderFI.cpp"

    # AssImp
    "src/apostate/ModelLoaderAssimp.h"
    "src/apostate/ModelLoaderAssimp.cpp"
)

target_include_directories(${target} PUBLIC ${IL_INCLUDE_DIR})
target_include_directories(${target} PUBLIC "src/")

target_link_libraries(
    ${target}
        assimp::assimp
        glfw
        glm
        glad
        nlohmann_json::nlohmann_json
        freeimage::FreeImage
)
