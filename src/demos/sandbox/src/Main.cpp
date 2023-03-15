
#include <array>
#include <iostream>
#include <map>
#include <string>
#include <xe/Timer.h>

#include "SandboxApp.h"
#include "Scene.h"

class FPSCounter {
public:
    //! must be called once per frame
    bool frame() {
        const int milliseconds = XE::Timer::getTick() - lastTime;

        if (milliseconds < 1000) {
            currentFrames++;

            return false;
        }
        else {
            fps = currentFrames;
            currentFrames = 0;
            lastTime = XE::Timer::getTick();

            return true;
        }
    }

    int getFPS() const {
        return fps;
    }

private:
    int lastTime = XE::Timer::getTick();
    int currentFrames = 0;
    int fps = 0;
};


int main(int argc, char **argv) {
    try {
        FPSCounter fpsCounter;

        auto app = std::make_unique<Sandbox::SandboxApp>(argc, argv);
        app->initialize();

        int lastTime = XE::Timer::getTick();

        while (!app->ShouldClose()) {
            int current = XE::Timer::getTick() - lastTime;
            float seconds = static_cast<float>(current) / 1000.0f;

            lastTime = XE::Timer::getTick();

            app->Update(seconds);
            app->Render();

            if (fpsCounter.frame()) {
                std::cout << "FPS: " << fpsCounter.getFPS() << std::endl;
            }
        }
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
