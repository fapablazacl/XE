
#include "Window.h"

#include <cstdio>
#include <map>
#include <glad/glad.h>

#include "bindings/imgui_impl_sdl2.h"
#include "bindings/imgui_impl_opengl3.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Window::Window() {}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::initialize() {
    if (initializeSDL()) {
        return false;
    }

    if (initializeOpenGL()) {
        return false;
    }

    context = ImGui::CreateContext();
    if (!context) {
        return false;
    }

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();

    return true;
}

int Window::initializeSDL() {
    std::printf("Initializing SDL video subsystem\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("Error while initialising SDL. SDL_Error: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    std::printf("Creating diplay window with 640 x 480 mode, windowed mode\n");
    const Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    window = SDL_CreateWindow( "Capybaria", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, flags );
    if( window == nullptr ) {
        std::printf( "Error while creating Window. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    return 0;
}

int Window::initializeOpenGL() {
    // OpenGL context configuration
    const int majorVersion = 4;
    const int minorVersion = 1;

    std::map<SDL_GLattr, int> sdlGlAttributes = {
        {SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion},
        {SDL_GL_CONTEXT_MINOR_VERSION, minorVersion},
        {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
        {SDL_GL_RED_SIZE, 8},
        {SDL_GL_GREEN_SIZE, 8},
        {SDL_GL_BLUE_SIZE, 8},
        {SDL_GL_ALPHA_SIZE, 8},
        {SDL_GL_DEPTH_SIZE, 16},
        {SDL_GL_BUFFER_SIZE, 32},
        {SDL_GL_DOUBLEBUFFER, 1}
    };

    for (const auto& pair : sdlGlAttributes) {
        SDL_GL_SetAttribute(pair.first, pair.second);
    }

    std::printf("Requested OpenGL context %d.%d\n", majorVersion, minorVersion);

    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::printf( "Error while creating OpenGL context. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    const int makeCurrentResult = SDL_GL_MakeCurrent(window, context);
    if (makeCurrentResult < 0) {
        std::printf( "Error while making OpenGL context current. SDL_Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

    return 0;
}

bool Window::pollInput() {
    SDL_Event e;

    while( SDL_PollEvent( &e ) ) {
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (e.type == SDL_QUIT) {
            return false;
        }
    }

    return true;
}

void Window::update() {}

void Window::prepareUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Window::drawUI() {
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(window);
}
