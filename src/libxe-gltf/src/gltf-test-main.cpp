
#include <stdexcept>

#include "fmt/printf.h"
#include "xe/app/Platform.h"
#include "xe/gl/RendererGL.h"

#include "GltfProcessor.h"

int main() {
    const char* filePath = "/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/models/capybara-01/capybara.glb";

    Platform platform;
    if (!platform.initialize()) {
        std::cerr << "Failed platform initialization." << std::endl;
        return EXIT_FAILURE;
    }

    auto renderer = xe::gl::RendererGL::create(platform.getGLProcAddressProcedure());

    GltfProcessor processor(renderer.get());
    if (!processor.process(filePath)) {
        std::cerr << "Failed gltf processing step." << std::endl;
        return EXIT_FAILURE;
    }

    bool running = true;

    while (running) {
        const InputState inputState = platform.pollInputState();
        running = !inputState.keyEscPress;

        renderer->clear(xe::gl::ClearParams()
            .color({0.0f, 0.0f, 0.0f, 1.0f})
            .depthX(1.0f));

        renderer->flush();
    }

    return EXIT_SUCCESS;
}
