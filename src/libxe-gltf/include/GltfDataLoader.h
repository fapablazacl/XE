
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

class GltfDataParser {
    cgltf_options options = {};

public:
    GltfDataParser() = default;

    explicit GltfDataParser(const cgltf_options &options) : options(options) {}

    [[nodiscard]]
    cgltf_data* parse(const std::string &filePath) const;
};

// Loads gltf data in the GPU
class GltfDataLoader {
    cgltf_data *data = nullptr;
    xe::gl::RendererGL *renderer = nullptr;

public:
    explicit GltfDataLoader(cgltf_data *data, xe::gl::RendererGL *renderer);

    std::vector<GltfMesh> loadAllMeshes();

private:
    GltfMeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive);

    GltfMesh createMesh(const cgltf_mesh *mesh);

    xe::gl::Buffer createIndexBuffer(const cgltf_accessor &accessor);

    xe::gl::Buffer createVertexBuffer(const cgltf_primitive &primitive);

    GLint mapAttributeName(const std::string &name);

    xe::gl::VertexArray createVertexArray(const cgltf_primitive &primitive);
};
