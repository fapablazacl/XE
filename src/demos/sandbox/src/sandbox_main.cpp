
#include <array>
#include <iostream>
#include <map>
#include <string>
#include <xe/Timer.h>

#include "FPSCounter.h"
#include "SandboxApp.h"
#include "Scene.h"

int main(int argc, char **argv) {
    try {
        FPSCounter fpsCounter;

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
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
