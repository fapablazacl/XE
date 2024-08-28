
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

struct FloorGeometry {
    int tilesInX = 0;
    int tilesInZ = 0;
    
    float tileSizeX = 0.0f;
    float tileSizeZ = 0.0f;

    int stripVertexCount = 0;

    GLuint vao = 0;
    GLuint vertexBuffer = 0;
};


FloorGeometry createFloorGeometry(const RendererGL &renderer, const GLint vertCoordLoc, const GLint vertColorLoc, const int tilesInX, const int tilesInZ, const float tileSizeX,
                                  const float tileSizeZ) {
    FloorGeometry floorGeometry;
    floorGeometry.tilesInX = tilesInX;
    floorGeometry.tilesInZ = tilesInZ;
    floorGeometry.tileSizeX = tileSizeX;
    floorGeometry.tileSizeZ = tileSizeZ;
    floorGeometry.stripVertexCount = 2 * (tilesInX + 1);

    std::vector<XE::Vector3> vertices{static_cast<size_t>(floorGeometry.stripVertexCount)};

    int j = 0;

    for (int i = 0; i < tilesInX + 1; i++) {
        vertices[2 * i] = XE::Vector3(i * tileSizeX, 0.0f, j * tileSizeZ);
        vertices[2 * i + 1] = XE::Vector3(i * tileSizeX, 0.0f, (j + 1) * tileSizeZ);
    }

    floorGeometry.vertexBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), vertices.size() * sizeof(XE::Vector3));

    glGenVertexArrays(1, &floorGeometry.vao);
    glBindVertexArray(floorGeometry.vao);

    glBindBuffer(GL_ARRAY_BUFFER, floorGeometry.vertexBuffer);
    glEnableVertexAttribArray(vertCoordLoc);
    glVertexAttribPointer(vertCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDisableVertexAttribArray(vertColorLoc);
    glVertexAttrib4f(vertColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(0);

    return floorGeometry;
}

void renderFloorGeometry(const FloorGeometry &floorGeometry, const GLint vertCoordZLoc, const GLint vertColourLoc) {
    glBindVertexArray(floorGeometry.vao);

    const XE::Vector4 colorFrom = {0.2f, 0.2f, 0.2f, 1.0f};
    const XE::Vector4 colorTo = {0.2f, 0.2f, 1.0f, 1.0f};

    for (int k = 0; k < floorGeometry.tilesInZ; k++) {
        const float z = k * floorGeometry.tileSizeZ;
        const float s = static_cast<float>(k) / (floorGeometry.tilesInZ - 1);
        const XE::Vector4 color = XE::lerp(colorFrom, colorTo, s);

        glVertexAttrib4fv(vertColourLoc, color.data());
        glVertexAttrib1f(vertCoordZLoc, z);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, floorGeometry.stripVertexCount);
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

in vec3 vertCoord;
in float vertCoordZ;
in vec4 vertColor;

out vec4 fragColor;

uniform mat4 uMvp;

void main() {
    // GLSL matrix-vector multiplication performs the correct linear-algebra operation
    // matrix-vector will multiply each row of the matrix with the column-vector at the right
    // vector-matrix will multiply the row-vector at the left with each column of the matrix.
    
    // vector-matrix should be more efficient because we are using matrices column-major order
    // wich is more efficient because of cache locality
    gl_Position = vec4(vertCoord.xy, vertCoord.z + vertCoordZ, 1.0) * uMvp;

    fragColor = vertColor;
}
)";


    const std::string fragmentShader = R"(
#version 410 core

in vec4 fragColor;

out vec4 finalColor;

void main() {
    finalColor = fragColor;
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

    const GLint vertColorLoc = glGetAttribLocation(program, "vertColor");
    assert(vertColorLoc >= 0);

    const GLint vertCoordZLoc = glGetAttribLocation(program, "vertCoordZ");
    assert(vertCoordZLoc >= 0);

    // prepare buffer 
    const int VERTEX_COLOUR = 3;

    const GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    const GLfloat colours[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
    };

    const GLuint vertexBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(GLfloat) * VERTEX_COLOUR * 3);
    const GLuint colourBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, colours, sizeof(GLfloat) * VERTEX_COLOUR * 4);

    GLuint triangleVao;
    glGenVertexArrays(1, &triangleVao);
    glBindVertexArray(triangleVao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(vertCoordLoc);
    glVertexAttribPointer(vertCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glEnableVertexAttribArray(vertColorLoc);
    glVertexAttribPointer(vertColorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);

    const auto floor = createFloorGeometry(renderer, vertCoordLoc, vertColorLoc, 10, 10, 1.0f, 1.0f);

    //Use Vsync
    std::printf("Configuring swap interval\n");
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        std::printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    //Hack to get window to stay up
    std::printf("Entering main loop\n");
    SDL_Event e;

    float angle = 0.0f;

    bool cameraMoveForward = false;
    bool cameraMoveBackward = false;
    bool cameraTurnLeft = false;
    bool cameraTurnRight = false;

    const float cameraSpeed = 1.0f;
    XE::Vector3 cameraPos;
    XE::Vector3 cameraLookAt;
    XE::Vector3 cameraDir = {0.0f, 0.0f, -1.0f};
    float cameraAngle = 0.0f;

    auto ticksLastTime = SDL_GetTicks64();

    bool quit = false;
    while( !quit ) {
        auto seconds = (SDL_GetTicks64()  - ticksLastTime) / 1000.0f;
        ticksLastTime = SDL_GetTicks64();

        assert(seconds >= 0.0f);

        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }

            switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    cameraTurnLeft = (e.type == SDL_KEYDOWN);
                    break;

                case SDLK_RIGHT:
                    cameraTurnRight = (e.type == SDL_KEYDOWN);
                    break;

                case SDLK_UP:
                    cameraMoveForward = (e.type == SDL_KEYDOWN);
                    break;

                case SDLK_DOWN:
                    cameraMoveBackward = (e.type == SDL_KEYDOWN);
                    break;
                }
                break;
            }
        }
        
        if ((angle += 100.0f * seconds) > 360.0f) {
            angle = std::fmod(angle, 360.0f);
        }

        if (cameraTurnLeft) {
            cameraAngle += 50.0f * seconds;
        }

        if (cameraTurnRight) {
            cameraAngle -= 50.0f * seconds;
        }

        cameraDir = XE::mat3RotationY(XE::radians(cameraAngle)) * XE::Vector3(0.0f, 0.0f, -1.0f);

        if (cameraMoveForward) {
            cameraPos += seconds * cameraSpeed * cameraDir;
        }

        if (cameraMoveBackward) {
            cameraPos -= seconds * cameraSpeed * cameraDir;
        }

        cameraDir.Y = cameraPos.Y = 0.25f;

        cameraLookAt = cameraPos + cameraDir;

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        glUseProgram(program);

        const int mvpLoc = glGetUniformLocation(program, "uMvp");

        const auto aspectRatio = SCREEN_HEIGHT / static_cast<float>(SCREEN_WIDTH);

        const auto viewProj = 
            XE::mat4Perspective(XE::radians(60.0f), aspectRatio, 0.0001f, 1000.0f) *
            XE::mat4LookAtRH(cameraPos, cameraLookAt, {0.0f, 1.0f, 0.0f});

        std::printf("cameraPos: %0.2f, %0.2f, %0.2f\n", cameraPos.X, cameraPos.Y, cameraPos.Z);
        std::printf("cameraDir: %0.2f, %0.2f, %0.2f\n", cameraDir.X, cameraDir.Y, cameraDir.Z);

        const auto triangleMatrix = viewProj * XE::mat4Translation({0.0f, 0.0f, 0.0f}) * XE::mat4RotationY(XE::radians(angle));

        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, triangleMatrix.data());

        glBindVertexArray(triangleVao);
        glVertexAttrib1f(vertCoordZLoc, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

        const auto floorMatrix = viewProj;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, floorMatrix.data());
        renderFloorGeometry(floor, vertCoordZLoc, vertColorLoc);

        glFlush();
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
