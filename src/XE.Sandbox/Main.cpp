
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE/Util/Timer.hpp>

#include "SandboxApp.hpp"

std::vector<std::string> ConvertArgs(int argc, char **argv) {
    std::vector<std::string> args;

    for (int i=1; i<argc; i++) {
        args.push_back(argv[i]);
    }

    return args;
}

int main(int argc, char **argv) {
    try {
        std::vector<std::string> args = ConvertArgs(argc, argv);
        auto app = XE::Sandbox::Application::Create(args);

        app->Initialize();

        int lastTime = XE::Util::Timer::GetTick();

        while (!app->ShouldClose()) {
            int current = XE::Util::Timer::GetTick() - lastTime;
            float seconds = static_cast<float>(current) / 1000.0f;

            lastTime = XE::Util::Timer::GetTick();

            app->Update(seconds);
            app->Render();
        }
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
