
#pragma once

#include <iostream>
#include <map>

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
    GLenum indexType = GL_UNSIGNED_INT;
};

struct GltfMesh {
    std::string name;
    std::vector<GltfMeshPrimitive> primitives;
};

using GltfAttributeName = std::string;
using ShaderAttributeName = std::string;

struct ShaderAttrib {
    std::string name;
    bool required = false;
    explicit ShaderAttrib(std::string name) : name(std::move(name)) {}
    explicit ShaderAttrib(std::string name, bool required) : name(std::move(name)), required(required) {}
};

using GltfAttributeMap = std::map<GltfAttributeName, ShaderAttrib>;

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
    xe::gl::Program program;
    GltfAttributeMap attributeMap;

public:
    explicit GltfDataLoader(cgltf_data *data, xe::gl::RendererGL *renderer, xe::gl::Program program, const GltfAttributeMap &attributeMap);

    std::vector<GltfMesh> loadAllMeshes();

private:
    GltfMeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive);

    GltfMesh createMesh(const cgltf_mesh *mesh);

    xe::gl::Buffer createIndexBuffer(const cgltf_accessor &accessor);

    xe::gl::Buffer createVertexBuffer(const cgltf_primitive &primitive);

    GLint computeAttributeLocation(const std::string &gltfAttributeName);

    xe::gl::VertexArray createVertexArray(const cgltf_primitive &primitive, xe::gl::Buffer vertexBuffer, xe::gl::Buffer indexBuffer);
};
