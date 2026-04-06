
#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <xe/graphics/Graphics.h>
#include <xe/math/Math.h>

//! Compute the offset from an object's base address, in bytes, for an specific field in a given type
#define OFFSETOF(type, field) reinterpret_cast<unsigned long long>(&(((type *)0)->field))

namespace demo {
    struct Vertex {
        xe::Vector3 coord;
        xe::Vector3 normal;
        xe::Vector4 color;

        Vertex() {
        }

        Vertex(xe::Vector3 coord, xe::Vector3 normal, xe::Vector4 color) : coord(coord), normal(normal), color(color) {
        }
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        xe::PrimitiveType primitive;
    };
} // namespace demo

namespace Sandbox {
    struct Material {};

    struct MeshPrimitive {
        const Material *material = nullptr;
        xe::PrimitiveType type = xe::PrimitiveType::PointList;
        std::vector<xe::Vector3> coords;
        std::vector<xe::Vector3> normals;
        std::vector<xe::Vector4> colors;
        std::vector<xe::Vector2> texCoords;
        std::vector<int> indices;

        xe::SubsetEnvelope getEnvelope() const {
            if (indices.size()) {
                return {type, 0, (int)indices.size()};
            }

            return {type, 0, (int)coords.size()};
        }
    };

    struct Mesh {
        std::string name;
        std::vector<MeshPrimitive> primitives;
    };

    struct SceneNode {
        std::string name;
        xe::Matrix4 localMatrix = xe::mat4Identity();
        std::vector<SceneNode> children;

        std::string meshName;
    };

    struct Scene {
        SceneNode root;
        std::map<std::string, Mesh> meshes;
    };

    class SceneLoader {
    public:
        virtual ~SceneLoader() {
        }
        virtual Scene load(const std::string &filePath) const = 0;
    };

    inline std::ostream &indent(std::ostream &os, const int indentation) {
        for (int i = 0; i < indentation; i++) {
            os << " ";
        }

        return os;
    }

    inline xe::Matrix4 makeMatrix(const float *data) {
        assert(data);

        xe::Matrix4 matrix;

        std::memcpy(matrix.data(), data, 16 * sizeof(float));

        return matrix;
    }

} // namespace Sandbox
