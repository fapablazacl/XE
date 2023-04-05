
#include "Camera.h"

void Camera::update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight) {
    const float speed = 2.0f;

    XE::Vector3 forward = {0.0f, 0.0f, 0.0f};

    if (moveForward) {
        forward = {0.0f, 0.0f, -1.0f};
    } else if (moveBackward) {
        forward = {0.0f, 0.0f, 1.0f};
    }

    XE::Vector3 side = {0.0f, 0.0f, 0.0f};
    if (turnLeft) {
        side = {-1.0f, 0.0f, 0.0f};
    } else if (turnRight) {
        side = {1.0f, 0.0f, 0.0f};
    }

    const XE::Vector3 displacement = seconds * speed * (forward + side);

    position += displacement;
    lookAt += displacement;
}
