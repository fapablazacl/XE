
#include <cstdio>
#include <xe/FPSCounter.h>
#include <SDL2/SDL.h>
#include <SDL_opengl.h>


int main(int argc, char *argv[]) {
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    std::printf("Initializing SDL video subsystem\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("Error while initialising SDL. SDL_Error: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    std::printf("Creating diplay window with 640 x 480 mode, windowed mode\n");
    auto window = SDL_CreateWindow( "Capybaria", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == nullptr ) {
        std::printf( "Error while creating Window. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    //Use OpenGL 3.1 core
    std::printf("Creating OpenGL context 3.1 core profile context\n");
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    auto context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::printf( "Error while creating OpenGL context. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    std::printf("OpenGL info:\n");
    std::printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    std::printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    std::printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
    std::printf("GL_EXTENSIONS: %s\n", glGetString(GL_EXTENSIONS));

    //Use Vsync
    std::printf("Configuring swap interval\n");
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    //Hack to get window to stay up
    std::printf("Entering main loop\n");
    SDL_Event e;

    bool quit = false;
    while( !quit ) {
        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }
        }

        glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glFlush();
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    /*
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
    */

    return EXIT_SUCCESS;
}
