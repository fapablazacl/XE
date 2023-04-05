
#ifndef __XE_GRAPHICS_VERTEX_HPP__
#define __XE_GRAPHICS_VERTEX_HPP__

#include <xe/math/Vector.h>

namespace XE {
    struct Vertex {
        Vector3 position;
        Vector3 Normal;
        Vector2 TexCoord;
    };
} // namespace XE

#endif
