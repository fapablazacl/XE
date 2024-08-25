
#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <map>
#include <iostream>
#include <cassert>

#include <xe/math/Vector.h>
#include <xe/math/Matrix.h>

#include "RendererGL.h"


static std::string hexstr(const GLenum value) {
    std::string str;
    str.resize(16, ' ');
    std::snprintf(str.data(), str.size(), "%x", value);

    return str;
}

static std::string stringval(const GLenum err) {
    switch (err) {
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_CONTEXT_LOST:
        return "GL_CONTEXT_LOST";

#if defined(GL_TABLE_TOO_LARGE)
    case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";
#endif

    default:
        return "UNKNOWN_ERR_CODE_" + hexstr(err);
    }
}

void GraphicsDeviceGL_callback(const char *name, void *, int, ...) {
    if (std::string(name) == "glGetError") {
        return;
    }

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        std::cerr << "Error while calling function " << name << std::endl;
        std::cerr << "Errors generated:" << std::endl;

        while (err != GL_NO_ERROR) {
            std::cerr << stringval(err) << std::endl;
            err = glGetError();
        }

        throw std::runtime_error("");
    }
}


int main(int /*argc*/, char */*argv*/[]) {
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    std::printf("Initializing SDL video subsystem\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("Error while initialising SDL. SDL_Error: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    std::printf("Creating diplay window with 640 x 480 mode, windowed mode\n");
    auto window = SDL_CreateWindow( "Capybaria", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    if( window == nullptr ) {
        std::printf( "Error while creating Window. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    // OpenGL context configuration
    const int majorVersion = 4;
    const int minorVersion = 1;

    std::map<SDL_GLattr, int> sdlGlAttributes = {
        {SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion},
        {SDL_GL_CONTEXT_MINOR_VERSION, minorVersion},
        {SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE},
    };

    for (const auto& pair : sdlGlAttributes) {
        SDL_GL_SetAttribute(pair.first, pair.second);
    }

    std::printf("Requested OpenGL context %d.%d\n", majorVersion, minorVersion);

    auto context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::printf( "Error while creating OpenGL context. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    const int makeCurrentResult = SDL_GL_MakeCurrent(window, context);
    if (makeCurrentResult < 0) {
        std::printf( "Error while making OpenGL context current. SDL_Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

    const int gladLoadResult = gladLoadGL();
    // const int gladLoadResult = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (!gladLoadResult) {
        std::printf("Error while initializing GLAD entry points");
        return EXIT_FAILURE;
    }
    
#ifndef NDEBUG
    glad_set_post_callback_gl(GraphicsDeviceGL_callback);
    glad_set_post_callback(GraphicsDeviceGL_callback);
#endif

    // initialize GL state, and render a single triangle
    const std::string vertexShader = R"(
#version 410 core

in vec2 vertCoord;

uniform mat4 uMvp;

void main() {
    gl_Position = uMvp * vec4(vertCoord, 0.0, 1.0);
}
)";


    const std::string fragmentShader = R"(
#version 410 core

out vec4 finalColor;

void main() {
    finalColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

    std::printf("OpenGL info:\n");
    std::printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    std::printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    std::printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
    std::printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    const RendererGL renderer;
    const GLuint program = renderer.createProgram({
        renderer.createShader(GL_VERTEX_SHADER, vertexShader), 
        renderer.createShader(GL_FRAGMENT_SHADER, fragmentShader)});

    if (!program) {
        return EXIT_FAILURE;
    }

    // get attrib location
    const GLint vertCoordLoc = glGetAttribLocation(program, "vertCoord");
    assert(vertCoordLoc >= 0);

    // prepare buffer 
    const GLfloat vertices[] = {
        0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f
    };
    const GLuint vertexBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(GLfloat) * 6);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(vertCoordLoc);
    glVertexAttribPointer(vertCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    //Use Vsync
    std::printf("Configuring swap interval\n");
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        std::printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    //Hack to get window to stay up
    std::printf("Entering main loop\n");
    SDL_Event e;

    float angle = 0.0f;

    auto ticksLastTime = SDL_GetTicks64();

    bool quit = false;
    while( !quit ) {
        auto seconds = (SDL_GetTicks64()  - ticksLastTime) / 1000.0f;
        ticksLastTime = SDL_GetTicks64();

        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }
        }

        if ((angle += 100.0f * seconds) > 360.0f) {
            angle = std::fmod(angle, 360.0f);
        }

        glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        glUseProgram(program);

        const int mvpLoc = glGetUniformLocation(program, "uMvp");

        auto mvp = 
            XE::mat4Identity() *
            XE::mat4RotationY(XE::radians(angle)) *
            XE::mat4LookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}) /**
            XE::mat4Perspective(XE::radians(60.0f), SCREEN_HEIGHT / static_cast<float>(SCREEN_WIDTH), 0.01f, 100.0f)*/;

        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data());

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

        glFlush();
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
