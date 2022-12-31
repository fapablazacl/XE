
#ifndef __XE_GRAPHICS_VERTEX_HPP__
#define __XE_GRAPHICS_VERTEX_HPP__

#include <xe/math/Vector.h>

namespace XE {
    struct Vertex {
        Vector3f position;
        Vector3f Normal;
        Vector2f TexCoord;
    };
}

#endif
