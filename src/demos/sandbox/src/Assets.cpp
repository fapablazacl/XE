
#include "Assets.h"

#include <xe/graphics/Subset.h>
#include <xe/math/Matrix.h>
#include <xe/math/Vector.h>

#include <iostream>

using XE::Vector2;
using XE::Vector3;
using XE::Vector4;

namespace Sandbox {
    static std::vector<Vector3> coordData = {{-1.5f, 1.5f, 0.0f}, {1.5f, 1.5f, 0.0f}, {-1.5f, -1.5f, 0.0f}, {1.5f, -1.5f, 0.0f}};

    static std::vector<Vector4> colorData = {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}};

    static std::vector<Vector3> normalData = {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}};

    static std::vector<Vector2> texCoordData = {{0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}};

    static std::vector<int> indexData = {0, 1, 2, 3};

    MeshPrimitive Assets::getSquareMeshPrimitive() {
        MeshPrimitive primitive;

        primitive.type = XE::PrimitiveType::TriangleList;
        primitive.coords = coordData;
        primitive.colors = colorData;
        primitive.normals = normalData;
        primitive.texCoords = texCoordData;
        primitive.indices = indexData;

        return primitive;
    }
} // namespace Sandbox
