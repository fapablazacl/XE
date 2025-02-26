
#include "Game.h"
#include "Util.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Game::Game() {}

Game::~Game() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::initialize() {
    if (initializeSDL()) {
        return false;
    }

    if (initializeOpenGL()) {
        return false;
    }

    return true;
}

int Game::initializeSDL() {
    std::printf("Initializing SDL video subsystem\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("Error while initialising SDL. SDL_Error: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    std::printf("Creating diplay window with 640 x 480 mode, windowed mode\n");
    window = SDL_CreateWindow( "Capybaria", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    if( window == nullptr ) {
        std::printf( "Error while creating Window. SDL_Error: %s\n", SDL_GetError() );

        return EXIT_FAILURE;
    }

    return 0;
}

int Game::initializeOpenGL() {
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

    // initialize GL state, and render a single triangle
    const std::string vertexShader = demo::loadTextFile("assets/capybaria.vert");
    const std::string fragmentShader = demo::loadTextFile("assets/capybaria.frag");

    renderer = RendererGL::create();

    std::vector<xe::gl::Shader> shaders = {
        renderer->createShader(GL_VERTEX_SHADER, vertexShader.c_str()),
        renderer->createShader(GL_FRAGMENT_SHADER, fragmentShader.c_str())
    };

    program = renderer->createProgram({shaders.data(), shaders.size()});

    if (!program.id) {
        return EXIT_FAILURE;
    }

    // get attrib location
    vertCoordLoc = program.getAttribLocation("vertCoord");
    vertColorLoc = program.getAttribLocation("vertColor");
    vertCoordZLoc = program.getAttribLocation("vertCoordZ");

    floor = createFloorGeometry(*renderer, vertCoordLoc, vertColorLoc, 10, 10, 1.0f, 1.0f);
    triangleVao = createTriangleGeometry(*renderer, vertCoordLoc, vertColorLoc);

    //Use Vsync
    std::printf("Configuring swap interval\n");
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        std::printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    return 0;
}


void Game::loop() {
    std::printf("Entering main loop\n");

    while (!actionState.quit) {
        seconds = timer.getFrameTimeInSeconds();

        pollInput();
        update();
        render();
    }
}


bool Game::pollInput() {
    SDL_Event e;

    while( SDL_PollEvent( &e ) ) {
        actionState.update(e);
    }

    return true;
}

void Game::update() {
    camera.update(seconds, actionState.actions);

    if ((angle += 30.0f * seconds) > 360.0f) {
        angle = std::fmod(angle, 360.0f);
    }
}


void Game::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    xe::gl::ClearParams clearParams;
    clearParams.colour = {0.0f, 0.0f, 0.0f, 1.0f};
    clearParams.depth = 1.0f;
    renderer->clear(clearParams);

    renderer->viewport({0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT});

    renderer->useProgram(program);

    const int mvpLoc = program.getUniformLocation( "uMvp");
    const auto viewProj = camera.getViewProj(SCREEN_WIDTH, SCREEN_HEIGHT);

    // render triangle
    Transformation transformation;
    transformation.rotation.X = XE::radians(angle);
    transformation.rotation.Y = XE::radians(angle);
    transformation.rotation.Z = XE::radians(angle);

    const auto triangleMatrix = viewProj * transformation.computeModelMatrix();

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, triangleMatrix.data());
    auto triangleVaoAttribData = 0.0f;
    auto triangleVaoAttrib = xe::gl::Attribute{vertCoordZLoc, xe::gl::AttributeDim::_1, xe::gl::AttributeType::Float};
    triangleVaoAttrib.data = &triangleVaoAttribData;

    auto triangleVaoPrimitive = xe::gl::VertexArrayPrimitive{0, 3, {&triangleVaoAttrib, 1}};
    auto triangleVaoPrimitiveMem = tcb::span<xe::gl::VertexArrayPrimitive>{&triangleVaoPrimitive, 1};
    renderer->draw(triangleVao, GL_TRIANGLE_STRIP, triangleVaoPrimitiveMem);

    // render floor geometry
    const auto floorMatrix = viewProj;
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, floorMatrix.data());
    renderFloorGeometry(floor, vertCoordZLoc, vertColorLoc);

    renderer->flush();

    SDL_GL_SwapWindow(window);
}
