
#include "Model.h"
#include "xe/gl/Renderer.h"
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Model::render(Renderer &renderer, const ShaderLocationMap &location) const {
    glm::mat4 const transform = glm::identity<glm::mat4>();

    renderNode(renderer, location, transform, rootNode);
}

void Model::renderNode(Renderer &renderer, const ShaderLocationMap &location, const glm::mat4 &parentTransform, const MeshNode &node) const {
    const glm::mat4 transform = parentTransform * node.transform;

    renderer.renderModelTransform(location, glm::value_ptr(transform));

    for (const size_t meshIndex : node.meshIndices) {
        const Mesh &mesh = meshes[meshIndex];

        if (mesh.material) {
            const Material &material = materials[*mesh.material];
            renderer.renderMaterial(renderer.program, material);
        } else {
            // TODO: Render default material
        }

        renderer.renderMesh(mesh);
    }

    for (const MeshNode &child : node.children) {
        renderNode(renderer, location, transform, child);
    }
}
