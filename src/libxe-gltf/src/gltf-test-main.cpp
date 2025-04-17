
#include <iostream>
#include "Asset_CGLTF.h"

static std::string to_string(cgltf_result result) {
    switch (result) {
    case cgltf_result_data_too_short: return "cgltf_result_data_too_short";
    case cgltf_result_unknown_format: return "cgltf_result_unknown_format";
    case cgltf_result_invalid_json: return "cgltf_result_invalid_json";
    case cgltf_result_invalid_gltf: return "cgltf_result_invalid_gltf";
    case cgltf_result_invalid_options: return "cgltf_result_invalid_options";
    case cgltf_result_file_not_found: return "cgltf_result_file_not_found";
    case cgltf_result_io_error: return "cgltf_result_io_error";
    case cgltf_result_out_of_memory: return "cgltf_result_out_of_memory";
    case cgltf_result_legacy_gltf: return "cgltf_result_legacy_gltf";
    case cgltf_result_success: return "cgltf_result_success";
    default: return "unknown enum" + std::to_string(result);
    }
}

void process_camera(cgltf_node *camera_node) {
    std::cout << "Node camera" << std::endl;
}

void process_light(cgltf_node *light_node) {
    std::cout << "Node light" << std::endl;
}

void process_material(const cgltf_material *material) {

}

void process_buffer_view(const cgltf_buffer_view *view) {
    std::cout << "Buffer view name: " << view->name << std::endl;
    std::cout << "Buffer view type: " << view->type << std::endl;
}

void process_accessor(cgltf_accessor *accessor) {
    process_buffer_view(accessor->buffer_view);

    std::cout << "Accessor count" << accessor->count << std::endl;
    std::cout << "Accessor extensions count" << accessor->extensions_count << std::endl;
}

void process_attribute(const cgltf_attribute &attribute) {
    std::cout << "Attribute name" << attribute.name << std::endl;
    std::cout << "Attribute type" << attribute.type << std::endl;

    process_accessor(attribute.data);
}

void process_primitive(const cgltf_primitive &primitive) {
    std::cout << "Primitive type" << primitive.type << std::endl;
    std::cout << "Primitive extension count" << primitive.extensions_count << std::endl;

    if (primitive.indices) {
        process_accessor(primitive.indices);
    }

    for (cgltf_size i = 0; i < primitive.attributes_count; i++) {
        process_attribute(primitive.attributes[i]);
    }

    if (primitive.material) {
        process_material(primitive.material);
    }
}

void process_mesh(cgltf_node *mesh_node) {
    std::cout << "Node mesh" << std::endl;

    cgltf_mesh *mesh = mesh_node->mesh;

    std::cout << mesh->name << std::endl;

    for (cgltf_size i = 0; i < mesh->primitives_count; i++) {
        process_primitive(mesh->primitives[i]);
    }
}

void process_node(cgltf_node *node) {
    std::cout << "Node name" << node->name << std::endl;
    std::cout << "Node has matrix " << node->has_matrix << std::endl;
    std::cout << "Node has translation " << node->has_translation << std::endl;
    std::cout << "Node has rotation " << node->has_rotation << std::endl;
    std::cout << "Node has scale " << node->has_scale << std::endl;

    if (node->mesh) {
        process_mesh(node);
    }

    if (node->light) {
        process_light(node);
    }

    if (node->camera) {
        process_camera(node);
    }

    std::cout << "Node children " << node->children_count << std::endl;
    for (cgltf_size ci = 0; ci < node->children_count; ci++) {
        process_node(node->children[ci]);
    }
}

void process_scene(cgltf_data *data, cgltf_scene *scene) {
    std::cout << "Scene name " << scene->name << std::endl;
    std::cout << "Scene node count " << scene->nodes_count << std::endl;

    for (cgltf_size ni = 0; ni < scene->nodes_count; ni++) {
        process_node(data->nodes + ni);
    }
}

int main() {
    const char* filePath = "/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/models/capybara-01/capybara.glb";

    std::cout << "Loading " << filePath << std::endl;

    cgltf_options options = {};
    cgltf_data *data = nullptr;
    cgltf_result result = cgltf_parse_file(&options, filePath, &data);

    if (result != cgltf_result_success) {
        std::cerr << "CGLTF: Couldn't load file '" << filePath << "'. Error code: " << to_string(result);
        return EXIT_FAILURE;
    }

    result = cgltf_load_buffers(&options, data, filePath);
    if (result != cgltf_result_success) {
        std::cerr << "CGLTF: error while loading buffers '" << filePath << "'. Error code: " << to_string(result);
        return EXIT_FAILURE;
    }

    std::cout << "Found " << data->scenes_count << " scenes" << std::endl;

    for (cgltf_size si = 0; si<data->scenes_count; si++) {
        process_scene(data, data->scenes + si);
    }

    return EXIT_SUCCESS;
}
