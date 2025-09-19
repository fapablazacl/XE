#pragma once

#include <SDL2/SDL.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

#include <xe/math/Matrix.h>
#include <xe/math/Vector.h>

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
        return XE::mat4Translation(position) * XE::mat4RotationX(rotation.X) * XE::mat4RotationY(rotation.Y) * XE::mat4RotationZ(rotation.Z);
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
        const auto seconds = static_cast<float>(SDL_GetTicks64() - lastTime) / 1000.0f;

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
