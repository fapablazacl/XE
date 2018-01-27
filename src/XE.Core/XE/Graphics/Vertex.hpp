
#ifndef __XE_GRAPHICS_VERTEX_HPP__
#define __XE_GRAPHICS_VERTEX_HPP__

#include <XE/Math/Vector.hpp>

namespace XE::Graphics {
    /**
     *
     */
    struct Vertex {
        XE::Math::Vector<float, 3> Position;
        XE::Math::Vector<float, 3> Normal;
        XE::Math::Vector<float, 3> TexCoord;
    };
}

#endif
