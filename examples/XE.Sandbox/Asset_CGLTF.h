
#pragma once

#include <string>
#include <vector>
#include <functional>

#include <XE/Graphics/Subset.hpp>
#include <XE/Math/Vector.hpp>
#include <XE/Math/Matrix.hpp>

#include <cgltf.h>

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
            return { nullptr, type, 0, (int)indices.size() };
        }
        
        return { nullptr, type, 0, (int)coords.size() };
    }
};


struct Mesh {
    std::string name;
    std::vector<MeshPrimitive> primitives;
};


using SceneNodeCallback = std::function<void (const XE::Matrix4f &transform, const std::string &meshName)>;


class Asset_CGLTF {
public:
    Asset_CGLTF();
    
    Asset_CGLTF(const std::string &filePath);
    
    ~Asset_CGLTF();
    
    void load(const std::string &filePath);
    
    void visitDefaultScene(SceneNodeCallback callback);
    
    std::vector<Mesh> getMeshes() const;
    
private:
    Mesh createMesh(const cgltf_mesh &cgltfMesh) const;
    
    MeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive) const;
    
    void visitAttribute(const int indentation, const cgltf_attribute &attrib);

    void visitPrimitive(const int indentation, const cgltf_primitive *primitive);

    void visitMesh(const int indentation, const cgltf_mesh *mesh);

    XE::Matrix4f computeNodeMatrix(const cgltf_node *node);

    void visitNode(const int indentation, const cgltf_node *node);

    void visitNode(const XE::Matrix4f &matrix, const cgltf_node *node);
    
    void visitScene(const cgltf_scene *scene);
    
private:
    cgltf_data *mData = nullptr;
    SceneNodeCallback mCallback;
};
