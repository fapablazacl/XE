#pragma once

#include <SDL2/SDL.h>

class Window {
public:
    Window();

    ~Window();

    bool initialize();

    void loop();

private:
    int initializeSDL();

    int initializeOpenGL();

    bool pollInput();

    void update();

    void render();

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
};
