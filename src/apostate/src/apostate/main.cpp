
#include "AssetPackage.h"
#include "ImageLoaderFI.h"
#include "Model.h"
#include "ModelLoaderAssimp.h"
#include "Platform.h"
#include "Renderer.h"
#include "TextureRepository.h"

#include <iostream>
#include <optional>
#include <vector>

#include <nlohmann/json.hpp>

using namespace apostate;


struct GameState {
    float angle = 0.0f;
    glm::vec3 playerPosition = {0.0f, 0.25f, 10.0f};
    glm::vec3 playerDirection = {0.0f, 0.0f, 0.0f};

    void updateOrientation(const bool turnLeft, const bool turnRight) {
        if (turnLeft) {
            angle += 0.02f;
        }
        else if (turnRight) {
            angle -= 0.02f;
        }
    }

    void updatePosition(const bool moveForward, const bool moveBackward) {
        // compute player direction
        const glm::mat4 rotationY = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3{0.0f, 1.0f, 0.0f});
        
        playerDirection = rotationY * glm::vec4{0.0f, 0.0f, -1.0f, 0.0f};

        // compute player movement
        if (! (moveForward && moveBackward)) {
            if (moveForward) {
                playerPosition += 0.075f * playerDirection;
            }
            else if (moveBackward) {
                playerPosition -= 0.075f * playerDirection;
            }
        }
    }
};


static GLuint createProgram(Renderer& renderer, AssetPackage &assetPackage, const std::string &vertFile, const std::string &fragFile) {
    const std::vector<GLuint> shaders {
        renderer.createShader(assetPackage.loadTextFile(vertFile), GL_VERTEX_SHADER), 
        renderer.createShader(assetPackage.loadTextFile(fragFile), GL_FRAGMENT_SHADER)
    };

    return renderer.createShaderProgram(shaders);
}


int main(int argc, char **argv) {
    std::cout << "Apostate Project" << std::endl;
    std::cout << "Copyright(c) 2022 Felipe Apablaza" << std::endl;

    /*
    if (argc < 2) {
        std::cerr << "Command Line Error: Missing model file path in the command line." << std::endl;
        return EXIT_FAILURE;
    }

    const std::string sceneFilePath = argv[1];
    */

    const std::string mediaFolder = "/Users/fapablaza/Dropbox/GameDev";

    // const std::string sceneFilePath = "Capybaras/capybara.glb";
    const std::string sceneFilePath = "Capybaras/capybara_01/source/capybara.glb";
    // const std::string sceneFilePath = "Capybaras/capybara_02/source/Capybara.fbx";
    // this have some reading errors
    // const std::string sceneFilePath = "Capybaras/capybara-low-poly/source/Capybara.fbx";
    // const std::string sceneFilePath = "Capybaras/carpincho-capybara-vrchat-avatar/source/Carpincho/Carpincho.obj";

    ImageLoaderFI imageLoader;
    TextureRepository textureRepository{imageLoader};
    
    Camera camera;
    
    Platform platform;
    if (!platform.initialize()) {
        std::cerr << "Failed platform initialization." << std::endl;
        return EXIT_FAILURE;
    }

    Renderer renderer{platform};
    if (!renderer.initialize()) {
        std::cerr << "Failed renderer initialization." << std::endl;
        return EXIT_FAILURE;
    }

    AssetPackage assetPackage;

    const GLuint 
    program = createProgram(renderer, assetPackage, "assets/gouraud.vert", "assets/gouraud.frag");
    if (! program) {
        std::cerr << "Failed to initialize Gouraud shader" << std::endl;
        return EXIT_FAILURE;
    }

    renderer.program = program;

    const ShaderLocationMap location = renderer.createShaderLocationMap(renderer.program);

    ModelLoaderAssimp modelLoader;
    Model model = modelLoader.createModel(mediaFolder + "/" + sceneFilePath, renderer, textureRepository, location);
    
    const Lighting lighting = {
        {0.1f, 0.1f, 0.1f, 0.1f}, {
            Light {
                glm::normalize(glm::vec3{0.5f, 1.0f, 0.25f}),
                glm::vec4{0.2f, 0.2f, 0.2f, 1.0f},
                glm::vec4{0.8f, 0.8f, 0.8f, 0.8f}
            },
            Light {
                glm::normalize(glm::vec3{-0.5f, -0.1f, 0.25f}),
                glm::vec4{0.0f, 0.0f, 0.0f, 1.0f},
                glm::vec4{0.8f, 0.8f, 0.8f, 0.8f}
            },
            Light {
                glm::normalize(glm::vec3{0.0f, 0.0f, -1.0f}),
                glm::vec4{0.2f, 0.2f, 0.2f, 1.0f},
                glm::vec4{0.8f, 0.8f, 0.8f, 0.8f}
            }
        }
    };

    bool running = true;

    GameState gameState;

    double lastTime = glfwGetTime();
    int fpsCount = 0;

    while (running) {
        double current = glfwGetTime() - lastTime;

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
        camera.fov = 45.0f;
        camera.aspect = platform.getAspectRatio();
        camera.znear = 0.1f;
        camera.zfar = 100.0f;

        camera.position = gameState.playerPosition;
        camera.lookAt = gameState.playerPosition + gameState.playerDirection;
        camera.up = glm::vec3{0.0f, 1.0f, 0.0f};

        // render the frame
        renderer.beginRenderFrame();
        renderer.renderCamera(location, camera);
        renderer.renderLighting(renderer.program, lighting);
        model.render(renderer, location);

        renderer.endRenderFrame();

        fpsCount ++;
    }

    return EXIT_SUCCESS;
};
