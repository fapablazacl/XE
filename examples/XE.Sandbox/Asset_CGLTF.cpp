
#include "Asset_CGLTF.h"

#include <iostream>
#include <XE/Math/Quaternion.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include "Common.h"


namespace Sandbox {
    static XE::Matrix4f computeNodeMatrix(const cgltf_node *node);
    
    static void visitAttribute(const int indentation, const cgltf_attribute &attrib) {
        Sandbox::indent(std::cout, indentation) << " visitAttribute: " << attrib.name << std::endl;
    }


    static void visitPrimitive(const int indentation, const cgltf_primitive *primitive) {
        assert(primitive);
        
        Sandbox::indent(std::cout, indentation) << "visitPrimitive: type " << primitive->type << std::endl;
        
        for (cgltf_size k=0; k<primitive->attributes_count; k++) {
            visitAttribute(indentation + 2, primitive->attributes[k]);
        }
    }


    static void visitMesh(const int indentation, const cgltf_mesh *mesh) {
        assert(mesh);
        
        Sandbox::indent(std::cout, indentation) << "visitMesh: " << mesh->name << std::endl;
        
        for (cgltf_size j=0; j<mesh->primitives_count; j++) {
            const cgltf_primitive &primitive = mesh->primitives[j];
            
            visitPrimitive(indentation + 2, &primitive);
        }
    }

    static void visitNode(const int indentation, const cgltf_node *node) {
        assert(node);
        
        Sandbox::indent(std::cout, indentation) << "visitNode: " << node->name << std::endl;
        
        const XE::Matrix4f nodeMatrix = computeNodeMatrix(node);
        
        if (node->mesh) {
            visitMesh(indentation + 2, node->mesh);
        }
        
        if (node->light) {
            // TODO: Add support.
        }
        
        for (cgltf_size i=0; i<node->children_count; i++) {
            visitNode(indentation + 2, node->children[i]);
        }
    }
}


namespace Sandbox {
    static MeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive) {
        MeshPrimitive meshPrimitive;
        
        if (primitive.material) {
            const cgltf_material &material = *primitive.material;
            
            int x = 0;
        }
        
        // map primitive type
        switch (primitive.type) {
            case cgltf_primitive_type_triangles:
                meshPrimitive.type = XE::PrimitiveType::TriangleList;
                break;
                
            default:
                meshPrimitive.type = XE::PrimitiveType::PointList;
        }
        
        // map indices
        if (primitive.indices) {
            const cgltf_accessor &accessor = *primitive.indices;
            const cgltf_buffer_view &bufferView = *accessor.buffer_view;
            const cgltf_buffer &buffer = *bufferView.buffer;
            
            cgltf_size offset = 0;
            
            for (cgltf_size i=0; i<accessor.count; i++) {
                auto elementData = (const int)(*(const uint32_t*)((std::byte*)buffer.data + bufferView.offset + accessor.offset + offset));
                
                meshPrimitive.indices.push_back(elementData);
                
                offset += accessor.stride;
            }
        }
        
        // map attributes
        for (cgltf_size i=0; i<primitive.attributes_count; i++) {
            const cgltf_accessor *accessor = primitive.attributes[i].data;
            const cgltf_buffer_view *bufferView = accessor->buffer_view;
            const cgltf_buffer *buffer = bufferView->buffer;
            
            const std::string attribName = primitive.attributes[i].name;
            
            if (attribName == "POSITION") {
                cgltf_size offset = 0;
                
                for (cgltf_size j=0; j<accessor->count; j++) {
                    auto elementData = (const XE::Vector3f*)((std::byte*)buffer->data + bufferView->offset + accessor->offset + offset);
                
                    meshPrimitive.coords.push_back(*elementData);
                    
                    offset += accessor->stride;
                }
            } else if (attribName == "NORMAL") {
                cgltf_size offset = 0;
                
                for (cgltf_size j=0; j<accessor->count; j++) {
                    auto elementData = (const XE::Vector3f*)((std::byte*)buffer->data + bufferView->offset + accessor->offset + offset);
                
                    meshPrimitive.normals.push_back(*elementData);
                    
                    offset += accessor->stride;
                }
                
            } else if (attribName == "TEXCOORD_0") {
                cgltf_size offset = 0;
                
                for (cgltf_size j=0; j<accessor->count; j++) {
                    auto elementData = (const XE::Vector2f*)((std::byte*)buffer->data + bufferView->offset + accessor->offset + offset);
                
                    meshPrimitive.texCoords.push_back(*elementData);
                    
                    offset += accessor->stride;
                }
            }
        }
        
        for (size_t i=0; i<meshPrimitive.coords.size(); i++) {
            meshPrimitive.colors.push_back({1.0f, 1.0f, 1.0f, 1.0f});
        }
        
        return meshPrimitive;
    }

    static Mesh createMesh(const cgltf_mesh &cgltfMesh) {
        Mesh mesh;
        
        mesh.name = cgltfMesh.name;
        
        for (cgltf_size i=0; i<cgltfMesh.primitives_count; i++) {
            MeshPrimitive primitive = createMeshPrimitive(cgltfMesh.primitives[i]);
            
            mesh.primitives.push_back(primitive);
        }
        
        return mesh;
    }

}


namespace Sandbox {
    Asset_CGLTF::Asset_CGLTF() {}


    Asset_CGLTF::Asset_CGLTF(const std::string &filePath) {
        load(filePath);
    }


    Asset_CGLTF::~Asset_CGLTF() {
        if (mData) {
            cgltf_free(mData);
        }
    }


    void Asset_CGLTF::load(const std::string &filePath) {
        cgltf_options options = {};
        
        this->~Asset_CGLTF();
        
        cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &mData);
        
        if (result == cgltf_result_success) {
            // load the binary data
            cgltf_load_buffers(&options, mData, filePath.c_str());
        }
    }


    void Asset_CGLTF::visitDefaultScene(SceneNodeCallback callback) {
        mCallback = callback;
        
        visitScene(mData->scene);
    }


    std::vector<Mesh> Asset_CGLTF::getMeshes() const {
        std::vector<Mesh> meshes;
        
        // load the meshes
        meshes.clear();
        for (cgltf_size i=0; i<mData->meshes_count; i++) {
            Mesh mesh = createMesh(mData->meshes[i]);
            
            meshes.push_back(mesh);
        }
        
        return meshes;
    }


    static XE::Matrix4f computeNodeMatrix(const cgltf_node *node) {
        assert(node);
        
        auto nodeMatrix = XE::Matrix4f::createIdentity();
        
        if (node->has_matrix == 1) {
            nodeMatrix = Sandbox::makeMatrix(node->matrix);
        } else {
            if (node->has_translation) {
                // TODO: Untested translation
                const auto t = XE::Vector3f{node->translation};
                nodeMatrix *= XE::Matrix4f::createTranslation(t);
            }
            
            if (node->has_rotation) {
                // TODO: Untested rotation
                // TODO: Add missing cases for angle = 0 and = 180º.
                const auto q = XE::Quaternion<float>{node->rotation};
                
                const float radians = std::acosf(q.W);
                const float inv_denom = 1.0f / std::sqrtf(1.0f - q.W*q.W);
                
                if (radians > 0.0f) {
                    const XE::Vector3f axis = q.V * inv_denom;
                    
                    const auto r = XE::Vector4f{node->rotation};
                    nodeMatrix *= XE::Matrix4f::createRotation(radians, axis);
                }
            }
            
            if (node->has_scale) {
                // TODO: Untested scale
                const auto s = XE::Vector3f{node->scale};
                nodeMatrix *= XE::Matrix4f::createScaling({s, 1.0f});
            }
        }
        
        return nodeMatrix;
    }


    void Asset_CGLTF::visitNode(const XE::Matrix4f &matrix, const cgltf_node *node) {
        assert(node);
        
        const XE::Matrix4f nodeMatrix = matrix * computeNodeMatrix(node);
        
        if (node->mesh) {
            mCallback(nodeMatrix, node->mesh->name);
        }
        
        if (node->light) {
            // TODO: Add support.
        }
        
        for (cgltf_size i=0; i<node->children_count; i++) {
            visitNode(nodeMatrix, node->children[i]);
        }
    }


    void Asset_CGLTF::visitScene(const cgltf_scene *scene) {
        auto transformMatrix = XE::Matrix4f::createIdentity();
        
        for (cgltf_size i=0; i<scene->nodes_count; i++) {
            visitNode(transformMatrix, scene->nodes[i]);
        }
        
        // std::cout << "Visiting scene " << scene->name << std::endl;
        //
        //     for (cgltf_size i=0; i<scene->nodes_count; i++) {
        //     visitNode(0, scene->nodes[i]);
        // }
    }


    Scene SceneLoaderGLTF::load(const std::string &filePath) const {
        return {};
    }
}
