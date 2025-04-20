
#include <stdexcept>

#include "fmt/printf.h"
#include "xe/app/Platform.h"
#include "xe/gl/RendererGL.h"

#include "GltfProcessor.h"

int main() {
    const char* filePath = "/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/models/capybara-01/capybara.glb";

    Platform platform;
    if (!platform.initialize("gltf viewer", 640, 480)) {
        std::cerr << "Failed platform initialization." << std::endl;
        return EXIT_FAILURE;
    }

    auto renderer = xe::gl::RendererGL::create(platform.getGLProcAddressProcedure());

    GltfProcessor processor(renderer.get());
    if (!processor.process(filePath)) {
        std::cerr << "Failed gltf processing step." << std::endl;
        return EXIT_FAILURE;
    }

    const auto vertexShaderSource = R"(
#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 vertCoord;
in vec4 vertColor;

out vec4 fragColor;

void main() {
    gl_Position = vec4(vertCoord, 1.0) * model * view * proj;
    fragColor = vertColor;
})";

    const auto fragmentShaderSource = R"(
#version 330

in vec4 fragColor;

out vec4 color;

void main() {
    color = fragColor;
})";

    std::vector<xe::gl::Shader> shaders = {
        renderer->createShader(GL_VERTEX_SHADER, vertexShaderSource),
        renderer->createShader(GL_FRAGMENT_SHADER, fragmentShaderSource)
    };

    auto program = renderer->createProgram(shaders);
    if (!program.id) {
        std::cerr << "Failed create program." << std::endl;
        return EXIT_FAILURE;
    }

    bool running = true;

    while (running) {
        const InputState inputState = platform.pollInputState();
        running = !inputState.keyEscPress;

        renderer->clear(xe::gl::ClearParams()
            .color({0.0f, 0.0f, 0.0f, 1.0f})
            .depthX(1.0f));

        renderer->useProgram(program);

        renderer->flush();
    }

    return EXIT_SUCCESS;
}
