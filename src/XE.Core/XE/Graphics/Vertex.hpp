
#ifndef __XE_GRAPHICS_VERTEX_HPP__
#define __XE_GRAPHICS_VERTEX_HPP__

#include <XE/Math/Vector.hpp>

namespace XE {
    struct Vertex {
        XE::Vector3f Position;
        XE::Vector3f Normal;
        XE::Vector2f TexCoord;
    };
}

#endif
