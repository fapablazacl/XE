
#pragma once

#include <vector>
#include <xe/graphics.h>
#include <xe/math.h>

#define OFFSETOF(type, field) reinterpret_cast<unsigned long long>(&(((type *)0)->field))

namespace demo {
    struct Vertex {
        XE::Vector3f coord;
        XE::Vector3f normal;
        XE::Vector4f color;

        Vertex() {}

        Vertex(XE::Vector3f coord, XE::Vector3f normal, XE::Vector4f color) : coord(coord), normal(normal), color(color) {}
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        XE::PrimitiveType primitive;
    };
} // namespace demo
