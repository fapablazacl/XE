
#include "Assets.hpp"

#include <XE/Math/Matrix.hpp>
#include <XE/Math/Vector.hpp>
#include <XE/Graphics/Subset.hpp>

#include <iostream>


namespace XE::Sandbox {
    std::vector<Mesh> Assets::loadModel(const std::string &assetFilePath) {
        Asset_CGLTF assetGLTF;
        
        assetGLTF.load(assetFilePath);
        
        return assetGLTF.getMeshes();
    }


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


    std::vector<Vector3f> Assets::coordData = {
        {-1.5f,  1.5f, 0.0f},
        { 1.5f,  1.5f, 0.0f},
        {-1.5f, -1.5f, 0.0f},
        { 1.5f, -1.5f, 0.0f}
    };

    std::vector<Vector4f> Assets::colorData = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}
    };

    std::vector<Vector3f> Assets::normalData = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}
    };

    std::vector<Vector2f> Assets::texCoordData = {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

    std::vector<int> Assets::indexData = {0, 1, 2, 3};
}
