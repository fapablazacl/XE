
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <XE/Graphics/Subset.hpp>
#include <XE/Math/Vector.hpp>
#include <string>
#include <vector>

namespace XE::Sandbox {
    struct Material {
        
    };


    struct MeshPrimitive {
        const Material *material = nullptr;
        XE::PrimitiveType type = XE::PrimitiveType::PointList;
        std::vector<XE::Vector3f> coords;
        std::vector<XE::Vector3f> normals;
        std::vector<XE::Vector4f> colors;
        std::vector<XE::Vector2f> texCoords;
        std::vector<int> indices;
        
        SubsetEnvelope getEnvelope() const {
            if (indices.size()) {
                return { nullptr, type, 0, (int)indices.size() };
            }
            
            return { nullptr, type, 0, (int)coords.size() };
        }
    };


    struct Mesh {
        std::vector<MeshPrimitive> primitives;
    };


    struct Assets {
        Assets() = delete;
        
        static std::vector<Vector3f> coordData;
        static std::vector<Vector4f> colorData;
        static std::vector<Vector3f> normalData;
        static std::vector<Vector2f> texCoordData;
        static std::vector<int> indexData;
        
        static void loadModel();
        
        static std::vector<Mesh> loadModel(const std::string &assetFilePath);
        
        static MeshPrimitive getSquareMeshPrimitive();
    };
}

#endif
