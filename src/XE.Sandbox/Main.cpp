
#include <array>
#include <string>
#include <iostream>
#include <map>

#include "SandboxApp.hpp"

int main(int argc, char **argv) {
    try {
        std::vector<std::string> args;

        for (int i=1; i<argc; i++) {
            args.push_back(argv[i]);
        }

        auto app = XE::Sandbox::Application::Create(args);

        app->Initialize();

        while (!app->ShouldClose()) {
            app->Update();
            app->Render();
        }
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
