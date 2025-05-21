
#pragma once

#include <iostream>
#include <map>

#include "GltfUtil.h"
#include "cgltf/cgltf.h"
#include "xe/ImageLoader.h"
#include "xe/gl/RendererGL.h"
#include "xe/graphics/ImageLoader.h"

namespace xe::gl {
    class RendererGL;
}

struct GltfMaterial {
    xe::gl::Texture texture;
    std::vector<xe::gl::TextureParameter> parameters;
};

class GltfTextureLoader {
public:
    GltfTextureLoader(const xe::gl::RendererGL* renderer, ImageLoader* imageLoader);

    xe::gl::Texture createTexture(const xe::gl::RendererGL* renderer, const cgltf_texture_view &texture) const;

private:
    const xe::gl::RendererGL* renderer = nullptr;
    const ImageLoader* imageLoader = nullptr;
};

struct GltfIndexData {
    xe::gl::Buffer buffer;
    GLenum type = GL_UNSIGNED_INT;
};

// Contains OpenGL objects with data loaded from a gltf mesh
struct GltfMeshPrimitive {
    // TODO: this should be a reference / pointer.
    GltfMaterial material;

    xe::gl::VertexArray vao;
    xe::gl::Buffer vertexBuffer;
    GLenum primitive = GL_NONE;
    GLsizei count = 0;
    std::optional<GltfIndexData> indexData;
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
    GltfTextureLoader *textureLoader = nullptr;
    xe::gl::Program program;
    GltfAttributeMap attributeMap;

public:
    explicit GltfDataLoader(cgltf_data *data, xe::gl::RendererGL *renderer, GltfTextureLoader *textureLoader, xe::gl::Program program, const GltfAttributeMap &attributeMap);

    std::vector<GltfMesh> loadAllMeshes();

private:
    GltfMeshPrimitive createMeshPrimitive(const cgltf_primitive &primitive);

    GltfMesh createMesh(const cgltf_mesh *mesh);

    xe::gl::Buffer createIndexBuffer(const cgltf_accessor &accessor);

    xe::gl::Buffer createVertexBuffer(const cgltf_primitive &primitive);

    GLint computeAttributeLocation(const std::string &gltfAttributeName);

    xe::gl::VertexArray createVertexArray(const cgltf_primitive &primitive, xe::gl::Buffer vertexBuffer, xe::gl::Buffer indexBuffer);
};
