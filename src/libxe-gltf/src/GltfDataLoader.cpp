
#include "GltfDataLoader.h"

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

GltfDataLoader::GltfDataLoader(cgltf_data *data, xe::gl::RendererGL *renderer) : data(data), renderer(renderer) {}

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

    // FIXME: Assuming that all of the attributes are referencing the same count of vertices
    const auto count = static_cast<GLsizei>(primitive.indices ? primitive.indices->count : primitive.attributes[0].data->count);
    const auto vao = createVertexArray(primitive);

    if (! vao.id) {
        std::cerr << "Could not create vertex array." << std::endl;
        return {};
    }

    GltfMeshPrimitive loadedMesh;
    loadedMesh.primitive = primitiveType;
    loadedMesh.vertexBuffer = vertexBuffer;
    loadedMesh.indexBuffer = indexBuffer;
    loadedMesh.vao = vao;
    loadedMesh.count = count;

    return loadedMesh;
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

GLint GltfDataLoader::mapAttributeName(const std::string &name) {
    return -1;
}

xe::gl::VertexArray GltfDataLoader::createVertexArray(const cgltf_primitive &primitive) {
    std::vector<xe::gl::Attribute> attributesGL;

    for (cgltf_size i = 0; i < primitive.attributes_count; i++) {
        const auto &attribute = primitive.attributes[i];
        const auto &accessor = *attribute.data;
        const auto &bufferView = *accessor.buffer_view;

        auto dataTypeGL = mapToAttributeDataType(accessor.component_type);

        std::cout << attribute.name << std::endl;

        if (!dataTypeGL) {
            std::cerr << "Could not map attribute " << attribute.name << " with accessor component type " << accessor.component_type << std::endl;
            return {};
        }

        xe::gl::Attribute attributeGL;
        attributeGL.index = mapAttributeName(attribute.name);
        attributeGL.offset = bufferView.offset;
        attributeGL.type = dataTypeGL.value();
        attributeGL.stride = static_cast<GLsizei>(bufferView.stride);
        attributeGL.normalized = accessor.normalized;
        attributesGL.push_back(attributeGL);

        if (attributeGL.index == -1) {
            std::cerr << "Could not map attribute " << attribute.name << ": it is unknown by the current shader program." << std::endl;
            return {};
        }
    }

    return renderer->createVertexArray({attributesGL.data(), attributesGL.size()}, xe::gl::Buffer());
}
