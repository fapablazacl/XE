
#include "Assets.hpp"

#include <XE/Math/Matrix.hpp>
#include <XE/Math/Vector.hpp>

#include <iostream>

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
            int x = 0;
            
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


    void Assets::loadModel() {
        cgltf_options options = {};
        
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, "media/models/spaceship_corridorhallway/scene.gltf", &data);
        
        if (result == cgltf_result_success) {
            visitScene(data->scene);
            
            // process meshes
            // std::cout << "Mesh Count: " << data->meshes_count << std::endl;
            
            // for (cgltf_size i=0; i<data->meshes_count; i++) {
                // std::cout << "Mesh " << (i + 1) << std::endl;
                //     visitMesh(data->meshes[i]);
            // }
            
            cgltf_free(data);
        }
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
