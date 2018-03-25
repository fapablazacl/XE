
#ifndef __XE_GRAPHICS_VERTEX_HPP__
#define __XE_GRAPHICS_VERTEX_HPP__

#include <XE/Math/Vector.hpp>

namespace XE::Graphics {
    struct Vertex {
        XE::Math::Vector3f Position;
        XE::Math::Vector3f Normal;
        XE::Math::Vector2f TexCoord;
    };
}

#endif
