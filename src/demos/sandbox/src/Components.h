
#include <xe/math/Vector.h>

#pragma once

struct Gravity {
    XE::Vector3f force;
};

struct RigidBody {
    XE::Vector3f velocity;
    XE::Vector3f acceleration;
};

struct Transform {
    XE::Vector3f position;
    XE::Vector3f rotation;
    XE::Vector3f scale;
};
