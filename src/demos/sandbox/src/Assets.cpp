
#include "Assets.hpp"

#include <xe/math/Matrix.h>
#include <xe/math/Vector.h>
#include <xe/graphics/Subset.h>

#include <iostream>

using XE::Vector2f;
using XE::Vector3f;
using XE::Vector4f;

namespace Sandbox {
    static std::vector<Vector3f> coordData = {
        {-1.5f,  1.5f, 0.0f},
        { 1.5f,  1.5f, 0.0f},
        {-1.5f, -1.5f, 0.0f},
        { 1.5f, -1.5f, 0.0f}
    };

    static std::vector<Vector4f> colorData = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}
    };

    static std::vector<Vector3f> normalData = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}
    };

    static std::vector<Vector2f> texCoordData = {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

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
}
