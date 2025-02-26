#pragma once


#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <map>
#include <iostream>
#include <cassert>

#include <xe/math/Vector.h>
#include <xe/math/Matrix.h>

#include "xe/gl/RendererGL.h"


using xe::gl::RendererGL;


struct FloorGeometry {
    int tilesInX = 0;
    int tilesInZ = 0;

    float tileSizeX = 0.0f;
    float tileSizeZ = 0.0f;

    int stripVertexCount = 0;

    xe::gl::VertexArray vao;

    xe::gl::Buffer vertexBuffer;
};


inline FloorGeometry createFloorGeometry(const xe::gl::RendererGL &renderer, const GLint vertCoordLoc, const GLint vertColorLoc, const int tilesInX, const int tilesInZ, const float tileSizeX,
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

    floorGeometry.vertexBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, { vertices.data(), vertices.size() * sizeof(XE::Vector3) });

    xe::gl::Attribute attribs[] = {
        xe::gl::Attribute{vertCoordLoc, xe::gl::AttributeDim::_3, xe::gl::AttributeType::Float, GL_FALSE, 0, floorGeometry.vertexBuffer, 0},
        xe::gl::Attribute{vertColorLoc, xe::gl::AttributeDim::_3, xe::gl::AttributeType::Float, GL_FALSE, 0, {}, 0}
    };

    floorGeometry.vao = renderer.createVertexArray({attribs, 1}, {});

    return floorGeometry;
}


inline void renderFloorGeometry(const FloorGeometry &floorGeometry, const GLint vertCoordZLoc, const GLint vertColourLoc) {
    glBindVertexArray(floorGeometry.vao.id);

    const XE::Vector4 colorFrom = {0.2f, 0.2f, 0.2f, 1.0f};
    const XE::Vector4 colorTo = {0.2f, 0.2f, 1.0f, 1.0f};

    for (int k = 0; k < floorGeometry.tilesInZ; k++) {
        const float z = static_cast<float>(k) * floorGeometry.tileSizeZ;
        const float s = static_cast<float>(k) / static_cast<float>((floorGeometry.tilesInZ - 1));
        const XE::Vector4 color = XE::lerp(colorFrom, colorTo, s);

        glVertexAttrib4fv(vertColourLoc, color.data());
        glVertexAttrib1f(vertCoordZLoc, z);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, floorGeometry.stripVertexCount);
    }
}


inline xe::gl::VertexArray createTriangleGeometry(const RendererGL &renderer, const GLint vertCoordLoc, const GLint vertColorLoc) {
    // prepare buffer
    const int VERTEX_COLOUR = 3;

    const GLfloat vertices[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};

    const GLfloat colours[] = {
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    const auto vertexBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, {vertices, sizeof(GLfloat) * VERTEX_COLOUR * 3});
    const auto colourBuffer = renderer.createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, {colours, sizeof(GLfloat) * VERTEX_COLOUR * 4});

    xe::gl::Attribute attribs[] = {
        xe::gl::Attribute{vertCoordLoc, xe::gl::AttributeDim::_3, xe::gl::AttributeType::Float, GL_FALSE, 0, vertexBuffer, 0},
        xe::gl::Attribute{vertColorLoc, xe::gl::AttributeDim::_3, xe::gl::AttributeType::Float, GL_FALSE, 0, colourBuffer, 0}
    };

    return renderer.createVertexArray({attribs, 1}, {});
}

enum GAME_ACTION {
    GAME_ACTION_NONE = 0x00,
    GAME_ACTION_CAMERA_TURN_LEFT = 0x0001,
    GAME_ACTION_CAMERA_TURN_RIGHT = 0x0002,
    GAME_ACTION_CAMERA_TURN_UP = 0x0004,
    GAME_ACTION_CAMERA_TURN_DOWN = 0x0008,
    GAME_ACTION_CAMERA_MOVE_FORWARD = 0x00010,
    GAME_ACTION_CAMERA_MOVE_BACKWARD = 0x0020,
    GAME_ACTION_CAMERA_MOVE_LEFT = 0x0040,
    GAME_ACTION_CAMERA_MOVE_RIGHT = 0x0080,

    GAME_ACTION_QUIT = 0x1000,
};


struct Camera {
    const float turnSpeed = 25.0f;
    const float movementSpeed = 1.0f;
    const XE::Vector3 up = {0.0f, 1.0f, 0.0f};
    XE::Vector3 position;
    XE::Vector3 lookAt;
    XE::Vector3 direction = {0.0f, 0.0f, -1.0f};
    float yaw = 0.0f;
    float pitch = 0.0f;

    void update(const float seconds, const int actions) {
        if (actions & GAME_ACTION_CAMERA_TURN_LEFT) {
            yaw += turnSpeed * seconds;
        }

        if (actions & GAME_ACTION_CAMERA_TURN_RIGHT) {
            yaw -= turnSpeed * seconds;
        }

        if (actions & GAME_ACTION_CAMERA_TURN_UP) {
            pitch += turnSpeed * seconds;

            if (pitch >= 80.0f) {
                pitch = 80.0f;
            }
        }

        if (actions & GAME_ACTION_CAMERA_TURN_DOWN) {
            pitch -= turnSpeed * seconds;

            if (pitch <= -80.0f) {
                pitch = -80.0f;
            }
        }

        // direction = xe::mat3RotationX(xe::radians(pitch)) * xe::mat3RotationY(xe::radians(yaw)) * xe::Vector3(0.0f, 0.0f, -1.0f);
        direction = XE::mat3Rotation(XE::radians(pitch), {1.0f, 0.0f, 0.0f}) * XE::mat3RotationY(XE::radians(yaw)) * XE::Vector3(0.0f, 0.0f, -1.0f);
        const auto cameraRight = XE::normalize(XE::cross(direction, up));

        if (actions & GAME_ACTION_CAMERA_MOVE_FORWARD) {
            position += seconds * movementSpeed * direction;
        }

        if (actions & GAME_ACTION_CAMERA_MOVE_BACKWARD) {
            position -= seconds * movementSpeed * direction;
        }

        if (actions & GAME_ACTION_CAMERA_MOVE_RIGHT) {
            position += seconds * movementSpeed * cameraRight;
        }

        if (actions & GAME_ACTION_CAMERA_MOVE_LEFT) {
            position -= seconds * movementSpeed * cameraRight;
        }

        direction.Y = position.Y = 0.25f;

        lookAt = position + direction;
    }

    XE::Matrix4 getViewProj(const int screenWidth, const int screenHeight) const {
        const auto aspectRatio = screenHeight / static_cast<float>(screenWidth);
        const auto proj = XE::mat4Perspective(XE::radians(60.0f), aspectRatio, 0.0001f, 1000.0f);
        const auto view = XE::mat4LookAtRH(position, lookAt, up);

        std::printf("cameraPos: %0.2f, %0.2f, %0.2f\n", position.X, position.Y, position.Z);
        std::printf("cameraDir: %0.2f, %0.2f, %0.2f\n", direction.X, direction.Y, direction.Z);

        return proj * view;
    }
};


struct Transformation {
    XE::Vector3 scale = {1.0f, 1.0f, 1.0f};
    XE::Vector3 position;
    XE::Vector3 rotation;

    XE::Matrix4 computeModelMatrix() const {
        return
            XE::mat4Translation(position) *
            XE::mat4RotationX(rotation.X) *
            XE::mat4RotationY(rotation.Y) *
            XE::mat4RotationZ(rotation.Z);
    }
};


struct ActionState {
    int actions = 0;
    bool quit = false;

    std::map<int, int> actionMap;

    ActionState() {
        actionMap = {
            {SDLK_LEFT, GAME_ACTION_CAMERA_TURN_LEFT},
            {SDLK_RIGHT, GAME_ACTION_CAMERA_TURN_RIGHT},
            {SDLK_UP, GAME_ACTION_CAMERA_TURN_UP},
            {SDLK_DOWN, GAME_ACTION_CAMERA_TURN_DOWN},
            {SDLK_a, GAME_ACTION_CAMERA_MOVE_LEFT},
            {SDLK_d, GAME_ACTION_CAMERA_MOVE_RIGHT},
            {SDLK_w, GAME_ACTION_CAMERA_MOVE_FORWARD},
            {SDLK_s, GAME_ACTION_CAMERA_MOVE_BACKWARD},
            {SDLK_ESCAPE, GAME_ACTION_QUIT},
        };
    }

    void update(const SDL_Event &e) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }

        if (e.type == SDL_MOUSEMOTION) {
            const auto xrel = e.motion.xrel;
            const auto yrel = e.motion.yrel;

            std::printf("MouseMotion: %d, %d", xrel, yrel);
        }

        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            auto it = actionMap.find(e.key.keysym.sym);
            if (it == actionMap.end()) {
                return;
            }

            const auto action = it->second;

            if (e.type == SDL_KEYDOWN) {
                actions |= action;
            }

            if (e.type == SDL_KEYUP) {
                actions &= ~action;
            }
        }

        // FIXME: an action map should not become a action handler
        if (actions & GAME_ACTION_QUIT) {
            quit = true;
        }
    }
};


struct Timer {
    Uint64 lastTime = SDL_GetTicks64();

    //! must be called one per frame
    float getFrameTimeInSeconds() {
        auto seconds = (SDL_GetTicks64() - lastTime) / 1000.0f;
        lastTime = SDL_GetTicks64();

        return seconds;
    }
};



class Game {
public:
    Game();

    ~Game();

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

    std::unique_ptr<RendererGL> renderer;

    xe::gl::Program program;
    GLint vertCoordLoc = -1;
    GLint vertColorLoc = -1;
    GLint vertCoordZLoc = -1;

    float angle = 0.0f;
    float seconds = 0.0f;

    FloorGeometry floor;
    xe::gl::VertexArray triangleVao;

    Camera camera;
    Timer timer;
    ActionState actionState;
};
