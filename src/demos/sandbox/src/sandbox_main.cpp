
#include <array>
#include <iostream>
#include <xe/Timer.h>

#include "SandboxApp.h"
#include <xe/FPSCounter.h>

int main(int argc, char **argv) {
    try {
        XE::FPSCounter fpsCounter;

        auto app = std::make_unique<Sandbox::SandboxApp>(argc, argv);
        app->initialize();

        int lastTime = XE::Timer::getTick();

        while (!app->shouldClose()) {
            int current = XE::Timer::getTick() - lastTime;
            float seconds = static_cast<float>(current) / 1000.0f;

            lastTime = XE::Timer::getTick();

            app->update(seconds);
            app->render();

            if (fpsCounter.frame()) {
                std::cout << "FPS: " << fpsCounter.getFPS() << std::endl;
            }
        }
    } catch (const std::exception &exp) {
        std::cerr << "Unmanaged exception caught" << std::endl;
        std::cerr << "    type: \"" << typeid(exp).name() << "\"" << std::endl;
        std::cerr << "    message: \"" << exp.what() << "\"" << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
