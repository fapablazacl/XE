
#include <xe/math/Vector.h>

#pragma once

struct Gravity {
    XE::Vector3 force;
};

struct RigidBody {
    XE::Vector3 velocity;
    XE::Vector3 acceleration;
};

struct Transform {
    XE::Vector3 position;
    XE::Vector3 rotation;
    XE::Vector3 scale;
};
