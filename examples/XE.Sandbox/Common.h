
#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include <XE/Math.h>
#include <XE/Graphics.h>

namespace Sandbox {
    struct Material {};

    struct MeshPrimitive {
        const Material *material = nullptr;
        XE::PrimitiveType type = XE::PrimitiveType::PointList;
        std::vector<XE::Vector3f> coords;
        std::vector<XE::Vector3f> normals;
        std::vector<XE::Vector4f> colors;
        std::vector<XE::Vector2f> texCoords;
        std::vector<int> indices;
        
        XE::SubsetEnvelope getEnvelope() const {
            if (indices.size()) {
                return { type, 0, (int)indices.size() };
            }
            
            return { type, 0, (int)coords.size() };
        }
    };


    struct Mesh {
        std::string name;
        std::vector<MeshPrimitive> primitives;
    };


    struct SceneNode {
        std::string name;
        XE::Matrix4f localMatrix = XE::Matrix4f::createIdentity();
        std::vector<SceneNode> children;

        std::string meshName;
    };


    struct Scene {
        SceneNode root;
        std::map<std::string, Mesh> meshes;
    };


    class SceneLoader {
    public:
        virtual ~SceneLoader() {}
        virtual Scene load(const std::string &filePath) const = 0;
    };

    inline std::ostream& indent(std::ostream &os, const int indentation) {
        for (int i=0; i<indentation; i++) {
            os << " ";
        }
        
        return os;
    }


    inline XE::Matrix4f makeMatrix(const float *data) {
        assert(data);
        
        XE::Matrix4f matrix;
        
        std::memcpy(matrix.data, data, 16 * sizeof(float));
        
        return matrix;
    }
}
