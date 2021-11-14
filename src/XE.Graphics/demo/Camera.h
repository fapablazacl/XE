
#pragma once 

#include <XE/Math.h>

struct Camera {
    XE::Vector3f position = {0.0f, 1.5f, 2.5f};
    XE::Vector3f lookAt = {0.0f, 1.5f, 0.0f};
    XE::Vector3f up = {0.0f, 1.0f, 0.0f};

    float fov = XE::radians(60.0f);
    float aspectRatio = 1.33333f;
    float znear = 0.01f;
    float zfar = 1000.0f;
    
    void update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight);
};
