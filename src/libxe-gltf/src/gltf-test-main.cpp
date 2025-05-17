
#include <stdexcept>
#include <array>

#include "fmt/printf.h"
#include "xe/app/Platform.h"
#include "xe/gl/RendererGL.h"

#include "GltfDataLoader.h"
#include "GltfProcessor.h"
#include "xe/math/Matrix.h"

const auto vertexShaderSource = R"(
#version 330

uniform mat4 modelViewProj;

in vec3 vertCoord;
in vec4 vertColor;

out vec4 fragColor;

void main() {
    gl_Position = vec4(vertCoord, 1.0) * modelViewProj;
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
})";

const auto fragmentShaderSource = R"(
#version 330

in vec4 fragColor;

out vec4 color;

void main() {
    color = fragColor;
})";

struct ShaderProgramUniformData {
    XE::Matrix4 projViewModel;

    [[nodiscard]]
    std::array<xe::gl::UniformMatrix, 1> mapUniforms(xe::gl::Program shaderProgram) const {
        return {
            xe::gl::makeUniform(shaderProgram.getUniformLocation("modelViewProj"), projViewModel),
        };
    }
};

int main() {
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;
    const char* filePath = "/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/models/capybara-01/capybara.glb";

    Platform platform;
    if (!platform.initialize("gltf viewer", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "Failed platform initialization." << std::endl;
        return EXIT_FAILURE;
    }

    auto renderer = xe::gl::RendererGL::create(platform.getGLProcAddressProcedure());

    std::vector<xe::gl::Shader> shaders = {
        renderer->createShader(GL_VERTEX_SHADER, vertexShaderSource),
        renderer->createShader(GL_FRAGMENT_SHADER, fragmentShaderSource)
    };

    auto program = renderer->createProgram(shaders);
    if (!program.id) {
        std::cerr << "Failed create program." << std::endl;
        return EXIT_FAILURE;
    }

    auto gltfParser = GltfDataParser{};
    auto gltfData = gltfParser.parse(filePath);

    if (! gltfData) {
        std::cerr << "Failed gltfParser.parse()." << std::endl;
        return EXIT_FAILURE;
    }

    auto gltfLoader = GltfDataLoader{gltfData, renderer.get(), program, {
        {"POSITION", ShaderAttrib("vertCoord")},
        // {"NORMAL", ShaderAttrib{"vertColor"}}
    }};

    const auto meshes = gltfLoader.loadAllMeshes();

    if (meshes.empty()) {
        std::cerr << "Meshes could not be loaded. " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << meshes.size() << " meshes were loaded" << std::endl;

    bool running = true;

    ShaderProgramUniformData uniformData;

    while (running) {
        const InputState inputState = platform.pollInputState();
        running = !inputState.keyEscPress;

        const auto proj = XE::mat4Perspective(XE::radians(60.0f), static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(SCREEN_WIDTH), 0.001f, 1000.0f);
        const auto view = XE::mat4LookAtRH({0.0f, 0.0f, -25.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        const auto model = XE::mat4Identity();

        uniformData.projViewModel = proj * view * model;

        // renderer->viewport({0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT});
        renderer->clear(xe::gl::ClearParams()
            .color({0.2f, 0.2f, 0.8f, 1.0f})
            .depthX(1.0f));

        renderer->useProgram(program);

        const auto uniforms = uniformData.mapUniforms(program);
        renderer->apply(uniforms);

        for (const auto &mesh : meshes) {
            for (const auto &primitive: mesh.primitives) {
                const xe::gl::VertexArrayPrimitive prims [] = {
                    {0, primitive.count}
                };

                if (primitive.indexBuffer.id) {
                    renderer->drawIndexed(primitive.vao, primitive.primitive, primitive.indexType, prims);
                }
                else {
                    renderer->draw(primitive.vao, primitive.primitive, prims);
                }
            }
        }

        renderer->flush();
        platform.swapBuffers();
    }

    return EXIT_SUCCESS;
}
