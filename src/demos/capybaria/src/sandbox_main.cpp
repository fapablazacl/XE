
#include <array>
#include <iostream>
#include <cstdio>

#include "SandboxApp.h"
#include <xe/FPSCounter.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>


int main(int argc, char *argv[]) {
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("Error while initialising SDL: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }



    SDL_Quit();


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
