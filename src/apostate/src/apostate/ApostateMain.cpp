
#include "xe/gl/Renderer.h"
#include "xe/gl/TextureRepository.h"

#include "AssetPackage.h"
#include "Model.h"
#include "ModelLoaderAssimp.h"
#include "Platform.h"
#include "xe/ImageLoader.h"
#include "xe/Logger.h"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>


using namespace apostate;

struct GameState {
    float angle = 0.0F;
    glm::vec3 playerPosition = {0.0F, 5.25F, 10.0F};
    glm::vec3 playerDirection = {0.0F, 0.0F, 0.0F};

    void updateOrientation(const bool turnLeft, const bool turnRight) {
        if (turnLeft) {
            angle += 0.02F;
        } else if (turnRight) {
            angle -= 0.02F;
        }
    }

    void updatePosition(const bool moveForward, const bool moveBackward) {
        // compute player direction
        const glm::mat4 rotationY = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3{0.0F, 1.0F, 0.0F});

        playerDirection = rotationY * glm::vec4{0.0F, 0.0F, -1.0F, 0.0F};

        // compute player movement
        if (!(moveForward && moveBackward)) {
            if (moveForward) {
                playerPosition += 0.075F * playerDirection;
            } else if (moveBackward) {
                playerPosition -= 0.075F * playerDirection;
            }
        }
    }
};

static GLuint createProgram(Renderer &renderer, AssetPackage &assetPackage, const std::string &vertFile, const std::string &fragFile) {
    const std::vector<GLuint> shaders{
        renderer.createShader(assetPackage.loadTextFile(vertFile), GL_VERTEX_SHADER),
        renderer.createShader(assetPackage.loadTextFile(fragFile), GL_FRAGMENT_SHADER)
    };

    return renderer.createShaderProgram(shaders);
}

int main(int  /*argc*/, char ** /*argv*/) {
    std::cout << "Apostate Project" << '\n';
    std::cout << "Copyright(c) 2022 Felipe Apablaza" << '\n';

    /*
    if (argc < 2) {
        std::cerr << "Command Line Error: Missing model file path in the command line." << std::endl;
        return EXIT_FAILURE;
    }

    const std::string sceneFilePath = argv[1];
    */

    const std::string mediaFolder = "/Users/fapablaza/Dropbox/GameDev";

    const std::map<std::string, std::string> modelFileMap = {
        {"capybara01", "Capybaras/capybara_01/source/capybara.glb"},
        {"capybara02", "Capybaras/capybara.glb"},
        /*
         *"Generic/the-bathroom-free/source/Old House scene.fbx",
        "Generic/phoenix-bird/source/fly.fbx",
        "Generic/abandoned-warehouse-interior-scene/abandoned_warehouse_-_interior_scene.glb",

        // these two fbxs have some reading errors
        // "Capybaras/capybara_02/source/Capybara.fbx",
        // "Capybaras/capybara-low-poly/source/Capybara.fbx",

        "Capybaras/carpincho-capybara-vrchat-avatar/source/Carpincho/Carpincho.obj"
        */
    };

    std::map<std::string, Transformation> const modelTransformationMap = {
        {"capybara01", Transformation{{1.0F, 1.0F, 1.0F}, {2.0F, 0.0F, 0.0F}}},
        {"capybara02", Transformation{{1.0F, 1.0F, 1.0F}, {-2.0F, 0.0F, 0.0F}}}
    };

    auto imageLoader = createImageLoader();
    TextureRepository textureRepository{*imageLoader};

    Camera camera;

    Platform platform;
    if (!platform.initialize()) {
        std::cerr << "Failed platform initialization." << '\n';
        return EXIT_FAILURE;
    }

    Renderer renderer{platform};
    if (!renderer.initialize()) {
        std::cerr << "Failed renderer initialization." << '\n';
        return EXIT_FAILURE;
    }

    AssetPackage assetPackage;

    const GLuint program = createProgram(renderer, assetPackage, "assets/gouraud.vert", "assets/gouraud.frag");
    if (program == 0u) {
        std::cerr << "Failed to initialize Gouraud shader" << '\n';
        return EXIT_FAILURE;
    }

    renderer.program = program;

    const ShaderLocationMap location = renderer.createShaderLocationMap(renderer.program);

    ModelLoaderAssimp modelLoader;

    std::map<std::string, Model> modelMap;

    for (const auto &pair : modelFileMap) {
        std::string modelPath;
        modelPath.append(mediaFolder);
        modelPath.append("/");
        modelPath.append(pair.second);

        Model const model = modelLoader.createModel(modelPath, renderer, textureRepository, location);

        modelMap.emplace(pair.first, model);
    }

    const Lighting lighting = {
        {0.1F, 0.1F, 0.1F, 0.1F},
        {Light{glm::normalize(glm::vec3{0.5F, 1.0F, 0.25F}), glm::vec4{0.2F, 0.2F, 0.2F, 1.0F}, glm::vec4{0.8F, 0.8F, 0.8F, 0.8F}},
         Light{glm::normalize(glm::vec3{-0.5F, -0.1F, 0.25F}), glm::vec4{0.0F, 0.0F, 0.0F, 1.0F}, glm::vec4{0.8F, 0.8F, 0.8F, 0.8F}},
         Light{glm::normalize(glm::vec3{0.0F, 0.0F, -1.0F}), glm::vec4{0.2F, 0.2F, 0.2F, 1.0F}, glm::vec4{0.8F, 0.8F, 0.8F, 0.8F}}}
    };

    bool running = true;

    GameState gameState;

    double lastTime = glfwGetTime();
    int fpsCount = 0;

    std::set<std::string> modelsNotLoaded;

    while (running) {
        double const current = glfwGetTime() - lastTime;

        if (current >= 1.0) {
            platform.setTitle("Apostate (Current FPS: " + std::to_string(fpsCount) + ")");
            fpsCount = 0;
            lastTime = glfwGetTime();
        }

        // collect input
        const InputState inputState = platform.pollInputState();

        // generate game actions
        running = !inputState.keyEscPress;
        gameState.updateOrientation(inputState.keyLeftPress, inputState.keyRightPress);
        gameState.updatePosition(inputState.keyUpPress, inputState.keyDownPress);

        // update scene graph
        camera.fov = 45.0F;
        camera.aspect = platform.getAspectRatio();
        camera.znear = 0.1F;
        camera.zfar = 100.0F;

        camera.position = gameState.playerPosition;
        camera.lookAt = gameState.playerPosition + gameState.playerDirection;
        camera.up = glm::vec3{0.0F, 1.0F, 0.0F};

        // render the frame
        renderer.beginRenderFrame();
        renderer.renderCamera(location, camera);
        renderer.renderLighting(renderer.program, lighting);

        for (const auto &modelTransformation : modelTransformationMap) {
            const auto &modelName = modelTransformation.first;
            const auto &transformation = modelTransformation.second;
            const auto it = modelMap.find(modelName);

            if (it == modelMap.end() && !modelsNotLoaded.contains(modelName)) {
                XE_LOG_WARNING("Model {} is not loaded. Skipping rendering (log once)\n", modelName);
                modelsNotLoaded.insert(modelName);
                continue;
            }

            auto &model = it->second;

            // HACK: overriding the model's original root transformation, for testing purposes
            model.rootNode.transform = transformation.computeMatrix();
            model.render(renderer, location);
        }

        renderer.endRenderFrame();

        fpsCount++;
    }

    return EXIT_SUCCESS;
};
