
#include "GltfDataLoader.h"

#include "xe/Logger.h"

GltfTextureLoader::GltfTextureLoader(const xe::gl::RendererGL* renderer, ImageLoader* imageLoader) : renderer(renderer), imageLoader(imageLoader) {}

xe::gl::Texture GltfTextureLoader::createTexture(const xe::gl::RendererGL* renderer, const cgltf_texture_view &textureView) const {
    std::cout << "Creating texture " << sanitizeString(textureView.texture->name) << std::endl;

    const auto mimeType = textureView.texture->image->mime_type;
    const auto buffer = textureView.texture->image->buffer_view->buffer->data;
    const auto offset = textureView.texture->image->buffer_view->offset;
    const auto size = textureView.texture->image->buffer_view->size;
    const auto imageFormat = parseImageFormat(mimeType);

    if (!imageFormat.has_value()) {
        std::cerr << "Failed to parse image format " << mimeType << std::endl;
        return {};
    }

    auto image = imageLoader->loadImage(addPointerOffset(buffer, offset), size, imageFormat.value());
    auto imageData = image->getData();

    GLenum internalFormat = GL_RGB;
    GLenum format = GL_RGB;

    switch (imageData.bpp) {
    case 24:
        internalFormat = GL_RGB;
        format = GL_BGR;
        break;

    case 32:
        internalFormat = GL_RGBA;
        format = GL_BGRA;
        break;
    default:
        XE_LOG_WARNING("TextureRepository::createTexture: Unsupported image bpp {}. Defaulting to GL_RGB\n", imageData.bpp);
    }

    auto clientImage = xe::gl::ClientTextureImage2D {
        {imageData.width, imageData.height},
        format, GL_UNSIGNED_BYTE,
        imageData.pixels
    };

    return renderer->createTexture(GL_TEXTURE_2D, internalFormat, clientImage, true, {});
}

cgltf_data* GltfDataParser::parse(const std::string &filePath) const {
    cgltf_data *data = nullptr;
    const auto filePathCstr = filePath.c_str();

    if (auto result = cgltf_parse_file(&options, filePathCstr, &data); result != cgltf_result_success) {
        std::cerr << "CGLTF: Couldn't load file '" << filePath << "'. Error code: " << to_string(result);
        return {};
    }

    if (auto result = cgltf_load_buffers(&options, data, filePathCstr); result != cgltf_result_success) {
        std::cerr << "CGLTF: error while loading buffers '" << filePath << "'. Error code: " << to_string(result);
        return {};
    }

    return data;
}

GltfDataLoader::GltfDataLoader(cgltf_data *data, xe::gl::RendererGL *renderer, GltfTextureLoader *textureLoader, xe::gl::Program program, const GltfAttributeMap &attributeMap)
    : data(data), renderer(renderer), textureLoader(textureLoader), program(program), attributeMap(attributeMap) {}

std::vector<GltfMesh> GltfDataLoader::loadAllMeshes() {
    std::vector<GltfMesh> meshes;
    meshes.reserve(data->meshes_count);

    for (cgltf_size i = 0; i < data->meshes_count; i++) {
        const auto mesh = createMesh(data->meshes + i);

        if (mesh.primitives.empty()) {
            std::cerr << "Could not create mesh" << std::endl;
            return {};
        }

        meshes.push_back(mesh);
    }

    return meshes;
}

GltfMeshPrimitive GltfDataLoader::createMeshPrimitive(const cgltf_primitive &primitive) {
    const auto primitiveType = mapToPrimitive(primitive.type);
    const auto vertexBuffer = createVertexBuffer(primitive);
    const auto indexBuffer = primitive.indices ? createIndexBuffer(*primitive.indices) : xe::gl::Buffer();
    const auto indexType = primitive.indices ? mapToGLDataType(primitive.indices->component_type).value_or(GL_NONE) : GL_NONE;

    // FIXME: Assuming that all of the attributes are referencing the same count of vertices
    const auto count = static_cast<GLsizei>(primitive.indices ? primitive.indices->count : primitive.attributes[0].data->count);
    const auto vao = createVertexArray(primitive, vertexBuffer, indexBuffer);

    if (! vao.id) {
        std::cerr << "Could not create vertex array." << std::endl;
        return {};
    }

    GltfMeshPrimitive meshPrimitive;
    meshPrimitive.primitive = primitiveType;
    meshPrimitive.vertexBuffer = vertexBuffer;
    meshPrimitive.vao = vao;
    meshPrimitive.count = count;
    meshPrimitive.material.texture = textureLoader->createTexture(renderer, primitive.material->pbr_metallic_roughness.base_color_texture);

    if (indexBuffer.id != 0) {
        meshPrimitive.indexData = {indexBuffer, indexType};
    }

    return meshPrimitive;
}

GltfMesh GltfDataLoader::createMesh(const cgltf_mesh *mesh) {
    std::vector<GltfMeshPrimitive> primitives = {};
    primitives.reserve(mesh->primitives_count);

    for (cgltf_size i = 0; i < mesh->primitives_count; i++) {
        const auto meshPrimitive = createMeshPrimitive(mesh->primitives[i]);

        if (!meshPrimitive.vao.id) {
            std::cerr << "Could not create mesh primitive array." << std::endl;
            return {};
        }

        primitives.push_back(meshPrimitive);
    }

    return { sanitizeString(mesh->name), primitives};
}

xe::gl::Buffer GltfDataLoader::createIndexBuffer(const cgltf_accessor &accessor) {
    const auto indicesOffset = accessor.buffer_view->offset;
    const auto indicesSize = accessor.buffer_view->size;
    const auto indexPtr = addPointerOffset(accessor.buffer_view->buffer->data, indicesOffset);

    return renderer->createBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, {indexPtr, indicesSize});
}

xe::gl::Buffer GltfDataLoader::createVertexBuffer(const cgltf_primitive &primitive) {
    // FIXME: Assuming that the attributes are stored in different regions of the same buffer

    const auto lastAttribIndex = primitive.attributes_count - 1;
    const auto vertexOffset = primitive.attributes[0].data->buffer_view->offset;
    const auto vertexSize = primitive.attributes[lastAttribIndex].data->buffer_view->offset;
    const auto vertexBufferView = primitive.attributes[0].data->buffer_view;

    const auto vertexPtr = addPointerOffset(vertexBufferView->buffer->data, vertexOffset);

    return renderer->createBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, {vertexPtr, vertexSize});
}

GLint GltfDataLoader::computeAttributeLocation(const std::string &gltfAttributeName) {
    const auto it = attributeMap.find(gltfAttributeName);
    if (it == attributeMap.end()) {
        std::cout << "Ignoring unused gltf attribute '" << gltfAttributeName << std::endl;
        return -1;
    }

    const auto shaderAttrib = it->second;
    const auto location = program.getAttribLocation(shaderAttrib.name.c_str());

    if (shaderAttrib.required && location == -1) {
        const auto msg = "Shader Attribute '" + shaderAttrib.name + "' does not exists";
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }

    return location;
}

xe::gl::VertexArray GltfDataLoader::createVertexArray(const cgltf_primitive &primitive, xe::gl::Buffer vertexBuffer, xe::gl::Buffer indexBuffer) {
    assert(vertexBuffer.id);

    std::vector<xe::gl::Attribute> attributesGL;

    for (cgltf_size i = 0; i < primitive.attributes_count; i++) {
        const auto &attribute = primitive.attributes[i];
        const auto &accessor = *attribute.data;
        const auto &bufferView = *accessor.buffer_view;

        auto dataTypeGL = mapToAttributeDataType(accessor.component_type);
        if (!dataTypeGL) {
            std::cerr << "Could not map attribute " << attribute.name << " with accessor component type " << accessor.component_type << std::endl;
            return {};
        }

        auto attribDimGL = mapToAttribDim(accessor.type);
        if (!attribDimGL) {
            std::cerr << "Could not map attribute" << accessor.name << " with accessor type " << accessor.type << std::endl;
            return {};
        }

        const auto location = computeAttributeLocation(attribute.name);

        if (location != -1) {
            xe::gl::Attribute attributeGL;
            attributeGL.index = location;
            attributeGL.offset = bufferView.offset;
            attributeGL.type = dataTypeGL.value();
            attributeGL.stride = static_cast<GLsizei>(bufferView.stride);
            attributeGL.normalized = accessor.normalized;
            attributeGL.buffer = vertexBuffer;
            attributeGL.size = attribDimGL.value();
            attributesGL.push_back(attributeGL);
        }
    }

    return renderer->createVertexArray({attributesGL.data(), attributesGL.size()}, indexBuffer);
}
