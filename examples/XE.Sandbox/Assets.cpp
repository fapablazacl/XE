
#include "Assets.hpp"

#include <XE/Math/Matrix.hpp>
#include <XE/Math/Vector.hpp>
#include <XE/Graphics/Subset.hpp>

#include <iostream>


namespace XE::Sandbox {
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
}
