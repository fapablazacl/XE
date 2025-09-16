#pragma once

#include <SDL2/SDL.h>

class Window {
public:
    Window();

    ~Window();

    bool pollInput();

    void update();

    void render();

    bool initialize();

private:
    int initializeSDL();

    int initializeOpenGL();

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
};
