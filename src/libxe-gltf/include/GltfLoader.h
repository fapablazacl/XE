
#pragma once

#include <iostream>

#include "xe/gl/RendererGL.h"
#include "cgltf/cgltf.h"
#include "GltfUtil.h"

namespace xe::gl {
    class RendererGL;
}

// Contains OpenGL objects with data loaded from a gltf mesh
struct GltfMeshPrimitive {
    xe::gl::VertexArray vao;
    xe::gl::Buffer vertexBuffer;
    xe::gl::Buffer indexBuffer;
    GLenum primitive = GL_NONE;
    GLsizei count = 0;
};

struct GltfMesh {
    std::string name;
    std::vector<GltfMeshPrimitive> primitives;
};

class GltfLoader {
    xe::gl::RendererGL *renderer = nullptr;

public:
    explicit GltfLoader(xe::gl::RendererGL *renderer);

    std::vector<GltfMesh> loadMeshes(const std::string &filePath);

private:
    GltfMeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive);

    GltfMesh createMesh(const cgltf_mesh *mesh);

    xe::gl::Buffer createIndexBuffer(const cgltf_accessor &accessor);

    xe::gl::Buffer createVertexBuffer(const cgltf_primitive &primitive);

    GLint mapAttributeName(const std::string &name);

    xe::gl::VertexArray createVertexArray(const cgltf_primitive &primitive);
};
