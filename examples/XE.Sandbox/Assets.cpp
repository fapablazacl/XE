
#include "Assets.hpp"

#include <XE/Math/Matrix.hpp>
#include <XE/Math/Vector.hpp>
#include <XE/Graphics/Subset.hpp>

#include <iostream>
#include <filesystem>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"


namespace XE::Sandbox {
    std::ostream& indent(std::ostream &os, const int indentation) {
        for (int i=0; i<indentation; i++) {
            os << " ";
        }
        
        return os;
    }


    XE::Matrix4f makeMatrix(const float *data) {
        assert(data);
        
        XE::Matrix4f matrix;
        
        std::memcpy(matrix.data, data, 16 * sizeof(float));
        
        return matrix;
    }


    class Asset_CGLTF {
    public:
        ~Asset_CGLTF() {
            if (mData) {
                cgltf_free(mData);
            }
        }
        
        void load(const std::string &filePath) {
            cgltf_options options = {};
            
            if (mData) {
                cgltf_free(mData);
            }
            
            cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &mData);
            
            if (result == cgltf_result_success) {
                // load the binary data
                cgltf_load_buffers(&options, mData, filePath.c_str());

                // load the meshes
                mMeshes.clear();
                for (cgltf_size i=0; i<mData->meshes_count; i++) {
                    Mesh mesh = createMesh(mData->meshes[i]);
                    
                    mMeshes.push_back(mesh);
                }
            }
        }
        
        void render() {
            // visitScene should be in the rendering stage
            visitScene(mData->scene);
        }
        
        
        std::vector<Mesh> getMeshes() const {
            return mMeshes;
        }
        
    private:
        Mesh createMesh(const cgltf_mesh &cgltfMesh) const {
            Mesh mesh;
            
            for (cgltf_size i=0; i<cgltfMesh.primitives_count; i++) {
                MeshPrimitive primitive = createMeshPrimitive(cgltfMesh.primitives[i]);
                
                mesh.primitives.push_back(primitive);
            }
            
            return mesh;
        }
        
        
        MeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive) const {
            MeshPrimitive meshPrimitive;
            
            switch (primitive.type) {
                case cgltf_primitive_type_triangles:
                    meshPrimitive.type = XE::PrimitiveType::TriangleList;
                    break;
                    
                default:
                    meshPrimitive.type = XE::PrimitiveType::PointList;
            }
            
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
            
            return meshPrimitive;
        }
        
        
        void visitAttribute(const int indentation, const cgltf_attribute &attrib) {
            indent(std::cout, indentation) << " visitAttribute: " << attrib.name << std::endl;
        }


        void visitPrimitive(const int indentation, const cgltf_primitive *primitive) {
            assert(primitive);
            
            indent(std::cout, indentation) << "visitPrimitive: type " << primitive->type << std::endl;
            
            for (cgltf_size k=0; k<primitive->attributes_count; k++) {
                visitAttribute(indentation + 2, primitive->attributes[k]);
            }
        }


        void visitMesh(const int indentation, const cgltf_mesh *mesh) {
            assert(mesh);
            
            indent(std::cout, indentation) << "visitMesh: " << mesh->name << std::endl;
            
            for (cgltf_size j=0; j<mesh->primitives_count; j++) {
                const cgltf_primitive &primitive = mesh->primitives[j];
                
                visitPrimitive(indentation + 2, &primitive);
            }
        }


        XE::Matrix4f computeNodeMatrix(const cgltf_node *node) {
            assert(node);
            
            auto nodeMatrix = XE::Matrix4f::createIdentity();
            
            if (node->has_matrix == 1) {
                nodeMatrix = makeMatrix(node->matrix);
            } else {
                if (node->has_translation) {
                    const auto t = XE::Vector3f{node->translation};
                    nodeMatrix *= XE::Matrix4f::createTranslation(t);
                }
                
                if (node->has_rotation) {
                    // TODO: Refactor this implementation. The correct math primitive is a quaternion.
                    const auto r = XE::Vector4f{node->rotation};
                    nodeMatrix *= XE::Matrix4f::createRotation(r.W, {r.X, r.Y, r.Z});
                }
                
                if (node->has_scale) {
                    const auto s = XE::Vector3f{node->scale};
                    nodeMatrix *= XE::Matrix4f::createScaling({s, 1.0f});
                }
            }
            
            return nodeMatrix;
        }


        void visitNode(const int indentation, const cgltf_node *node) {
            assert(node);
            
            indent(std::cout, indentation) << "visitNode: " << node->name << std::endl;
            
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


        void visitScene(const cgltf_scene *scene) {
            std::cout << "Visiting scene " << scene->name << std::endl;
            
            for (cgltf_size i=0; i<scene->nodes_count; i++) {
                visitNode(0, scene->nodes[i]);
            }
        }
        
    private:
        cgltf_data *mData = nullptr;
        std::map<std::string, Mesh> mMeshesByName;
        std::vector<Mesh> mMeshes;
    };


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
