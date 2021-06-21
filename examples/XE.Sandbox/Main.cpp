
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE/Timer.h>

#include "SandboxApp.hpp"


int main(int argc, char **argv) {
    try {
        auto app = Sandbox::Application::create();

        app->Initialize();

        int lastTime = XE::Timer::getTick();

        while (!app->ShouldClose()) {
            int current = XE::Timer::getTick() - lastTime;
            float seconds = static_cast<float>(current) / 1000.0f;

            lastTime = XE::Timer::getTick();

            app->Update(seconds);
            app->Render();
        }
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
