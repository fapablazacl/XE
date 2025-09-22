#pragma once

#include <SDL2/SDL.h>
#include <imgui.h>

class Window {
public:
    Window();

    ~Window();

    static bool pollInput();

    void update();

    void swapBuffers();

    bool initialize();

    // called once per frame, just after the buffers are cleared
    static void prepareUI();

    // called once per frame
    static void drawUI();

private:
    int initializeSDL();

    int initializeOpenGL();

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
    ImGuiContext *imGuiContext = nullptr;
};
