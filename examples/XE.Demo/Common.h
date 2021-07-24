
#pragma once 

#include <XE/Math.h>
#include <XE/Graphics.h>
#include <vector>

namespace demo {
    struct Vertex {
        XE::Vector3f coord;
        XE::Vector4f color;
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        XE::PrimitiveType primitive;
    };
}
