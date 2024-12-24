
#pragma once 

#include "Renderer.h"

struct MeshNode {
    glm::mat4 transform = glm::identity<glm::mat4>();
    std::vector<uint32_t> meshIndices;
    std::vector<MeshNode> children;
};

struct Model {
    void render(Renderer &renderer, const ShaderLocationMap &location) const;

    void renderNode(
        Renderer &renderer, 
        const ShaderLocationMap &location,
        const glm::mat4 &parentTransform,
        const MeshNode &node
    ) const;

    MeshNode rootNode;
    std::vector<Mesh> meshes;
    std::vector<GLuint> textures;
    std::vector<Material> materials;
};

