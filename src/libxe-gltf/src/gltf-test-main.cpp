
#include <stdexcept>
#include <array>

#include "fmt/printf.h"
#include "xe/app/Platform.h"
#include "xe/gl/RendererGL.h"

#include "GltfDataLoader.h"
#include "GltfProcessor.h"
#include "xe/ImageLoader.h"
#include "xe/Timer.h"
#include "xe/math/Matrix.h"

const auto vertexShaderSource = R"(
#version 330 core

uniform mat4 modelViewProj;
uniform mat4 model;

uniform float seconds;

in vec3 vertCoord;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec4 fragColor;
out vec2 fragTexCoord;

float fmod(float x, float y) {
    return x - y * floor(x / y);
}

float wave(float seconds) {
    return (cos(seconds) + 1.0) / 2.0 * 0.5 + 0.5;
}

vec3 computeLightingColour() {
    return vec3(wave(seconds), wave(seconds + 0.5), wave(seconds - 0.5));
}

void main() {
    vec3 lightDirection[4];
    lightDirection[0] = normalize(vec3(0.5, 0.3, 0.4));
    lightDirection[1] = normalize(vec3(-0.5, 0.3, -0.4));
    lightDirection[2] = normalize(vec3(0.5, -0.3, 0.4));
    lightDirection[3] = normalize(vec3(-0.5, -0.3, -0.4));

    gl_Position = vec4(vertCoord, 1.0) * modelViewProj;

    vec4 color = vec4(0.0);
    for (int i = 0; i < 4; i++) {
        color += vec4(computeLightingColour() * max(dot(lightDirection[i], vertNormal), 0.0), 1.0);
    }
    fragColor = color;

    fragTexCoord = vertTexCoord;
})";

const auto fragmentShaderSource = R"(
#version 330 core

uniform sampler2D diffuseTexture;

in vec4 fragColor;
in vec2 fragTexCoord;

out vec4 color;

void main() {
    color = fragColor * texture(diffuseTexture, fragTexCoord);
})";

struct ShaderProgramUniformData {
    XE::Matrix4 projViewModel = XE::mat4Identity();

    XE::Matrix4 model = XE::mat4Identity();

    // The Texture Unit which contains the diffuse texture
    GLint diffuseTexture = 0;

    float seconds = 0.0f;

    [[nodiscard]]
    std::vector<xe::gl::UniformMatrix> mapMatrixUniforms(xe::gl::Program shaderProgram) const {
        return {
            xe::gl::makeUniform(shaderProgram.getUniformLocation("modelViewProj"), projViewModel),
        };
    }

    [[nodiscard]]
    std::vector<xe::gl::Uniform> mapUniforms(xe::gl::Program shaderProgram) const {
        return {
            xe::gl::makeUniform(shaderProgram.getUniformLocation("diffuseTexture"), diffuseTexture),
            xe::gl::makeUniform(shaderProgram.getUniformLocation("seconds"), seconds),
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

    auto imageLoader = createImageLoader();
    auto gltfTextureLoader = GltfTextureLoader{renderer.get(), imageLoader.get()};

    auto gltfLoader = GltfDataLoader{gltfData, renderer.get(), &gltfTextureLoader, program, {
        {"POSITION", ShaderAttrib("vertCoord")},
        {"NORMAL", ShaderAttrib{"vertNormal"}},
        {"TEXCOORD_0", ShaderAttrib{"vertTexCoord"}}
    }};

    const auto meshes = gltfLoader.loadAllMeshes();

    if (meshes.empty()) {
        std::cerr << "Meshes could not be loaded. " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << meshes.size() << " meshes were loaded" << std::endl;

    bool running = true;
    float angle = 0.0f;

    ShaderProgramUniformData uniformData;

    const float startSeconds = static_cast<float>(XE::Timer::getTick()) / 1000.0f;

    while (running) {
        uniformData.seconds = (static_cast<float>(XE::Timer::getTick()) / 1000.0f) - startSeconds;

        const InputState inputState = platform.pollInputState();
        running = !inputState.keyEscPress;

        const auto proj = XE::mat4Perspective(XE::radians(60.0f), static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(SCREEN_WIDTH), 0.001f, 1000.0f);
        const auto view = XE::mat4LookAtRH({0.0f, 0.0f, -25.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        const auto model = XE::mat4RotationY(angle += 0.005f);

        uniformData.model = model;
        uniformData.projViewModel = proj * view * model;

        const std::vector<xe::gl::CapabilityStatus> renderState = {
            {GL_DEPTH_TEST, GL_TRUE},
            {GL_CULL_FACE, GL_TRUE}
        };

        renderer->bindRenderState(renderState);
        renderer->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, XE::Vector4{0.2f, 0.2f, 0.8f, 1.0f}, {}, {});
        renderer->useProgram(program);

        renderer->bindRenderState(uniformData.mapUniforms(program));
        renderer->bindRenderState(uniformData.mapMatrixUniforms(program));

        for (const auto &mesh : meshes) {
            for (const auto &meshSubset: mesh.primitives) {
                const xe::gl::VertexArrayPrimitive prims [] = {
                    {0, meshSubset.count}
                };

                xe::gl::TextureLayer layer;
                layer.texture = meshSubset.material.texture;

                glActiveTexture(GL_TEXTURE0);
                renderer->bindRenderState({&layer, 1});

                if (meshSubset.indexData.has_value()) {
                    renderer->draw(meshSubset.vao, meshSubset.primitive, prims, meshSubset.indexData->type);
                }
                else {
                    renderer->draw(meshSubset.vao, meshSubset.primitive, prims);
                }
            }
        }

        renderer->flush();
        platform.swapBuffers();
    }

    return EXIT_SUCCESS;
}
