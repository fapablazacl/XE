
#include <iostream>
#include <cgltf/cgltf.h>

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

void process_camera(cgltf_camera *camera) {
    std::cout << "camera node" << std::endl;
}

void process_light(cgltf_light *light) {
    std::cout << "light node" << std::endl;
}

void process_material(const cgltf_material *material) {
    std::cout << "material node" << std::endl;
}

void process_buffer_view(const cgltf_buffer_view *view) {
    // std::cout << "Buffer view name: " << (view->name ? view->name : "<noname>") << std::endl;
    // std::cout << "Buffer view type: " << view->type << std::endl;
}

void process_accessor(cgltf_accessor *accessor) {
    // std::cout << "Accessor count: " << accessor->count << std::endl;
    // std::cout << "Accessor extensions count: " << accessor->extensions_count << std::endl;
    // std::cout << "Accessor have buffer_view: " << (accessor->buffer_view != nullptr) << std::endl;

    if (accessor->buffer_view) {
        process_buffer_view(accessor->buffer_view);
    }
}

void process_attribute(const cgltf_attribute &attribute) {
    std::cout << "Attribute name" << attribute.name << std::endl;
    std::cout << "Attribute type" << attribute.type << std::endl;

    process_accessor(attribute.data);
}

void process_primitive(const cgltf_primitive &primitive) {
    std::cout << "Primitive type " << primitive.type << std::endl;
    std::cout << "Primitive extension count " << primitive.extensions_count << std::endl;

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

void process_mesh(cgltf_mesh *mesh) {
    std::cout << "Node mesh " << mesh->name << std::endl;
    std::cout << "Node primitives count " << mesh->primitives_count << std::endl;

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
        process_mesh(node->mesh);
    }

    if (node->light) {
        process_light(node->light);
    }

    if (node->camera) {
        process_camera(node->camera);
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

void process_animation(cgltf_animation *animation) {

}

void process_skins(cgltf_skin *skin) {

}

void process_texture(cgltf_texture *texture) {

}

void process_image(cgltf_image *image) {

}

void process_sampler(cgltf_sampler *sampler) {

}

void process_buffer(cgltf_buffer *buffer) {

}

void process_data_extension(cgltf_extension *extension) {

}

void process_extensions_required(char **str) {

}

void process_extensions_used(char **str) {

}

void process_variant(cgltf_material_variant * material_variant) {

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

    std::cout << "Found " << data->lights_count << " lights" << std::endl;
    for (cgltf_size i = 0; i < data->lights_count; i++) {
        process_light(data->lights + i);
    }

    std::cout << "Found " << data->cameras_count << " cameras" << std::endl;
    for (cgltf_size i = 0; i < data->cameras_count; i++) {
        process_camera(data->cameras + i);
    }

    std::cout << "Found " << data->meshes_count << " meshes" << std::endl;
    for (cgltf_size i = 0; i < data->meshes_count; i++) {
        process_mesh(data->meshes + i);
    }

    std::cout << "Found " << data->animations_count << " animations" << std::endl;
    for (cgltf_size i = 0; i < data->animations_count; i++) {
        process_animation(data->animations + i);
    }

    std::cout << "Found " << data->skins_count << " skins" << std::endl;
    for (cgltf_size i = 0; i < data->skins_count; i++) {
        process_skins(data->skins + i);
    }

    std::cout << "Found " << data->textures_count << " textures" << std::endl;
    for (cgltf_size i = 0; i < data->textures_count; i++) {
        process_texture(data->textures + i);
    }

    std::cout << "Found " << data->images_count << " images" << std::endl;
    for (cgltf_size i = 0; i < data->images_count; i++) {
        process_image(data->images + i);
    }

    std::cout << "Found " << data->samplers_count << " samplers" << std::endl;
    for (cgltf_size i = 0; i < data->samplers_count; i++) {
        process_sampler(data->samplers + i);
    }

    std::cout << "Found " << data->accessors_count << " accessors" << std::endl;
    for (cgltf_size i = 0; i < data->accessors_count; i++) {
        process_accessor(data->accessors + i);
    }

    std::cout << "Found " << data->buffers_count << " buffers" << std::endl;
    for (cgltf_size i = 0; i < data->buffers_count; i++) {
        process_buffer(data->buffers + i);
    }

    std::cout << "Found " << data->buffer_views_count << " buffer views" << std::endl;
    for (cgltf_size i = 0; i < data->buffer_views_count; i++) {
        process_buffer_view(data->buffer_views + i);
    }

    std::cout << "Found " << data->data_extensions_count << " data extensions" << std::endl;
    for (cgltf_size i = 0; i < data->data_extensions_count; i++) {
        process_data_extension(data->data_extensions + i);
    }

    std::cout << "Found " << data->extensions_required_count << " extensions required" << std::endl;
    for (cgltf_size i = 0; i < data->extensions_required_count; i++) {
        process_extensions_required(data->extensions_required + i);
    }

    std::cout << "Found " << data->extensions_used_count << " extensions used" << std::endl;
    for (cgltf_size i = 0; i < data->extensions_used_count; i++) {
        process_extensions_used(data->extensions_used + i);
    }

    std::cout << "Found " << data->variants_count << " variants" << std::endl;
    for (cgltf_size i = 0; i < data->variants_count; i++) {
        process_variant(data->variants + i);
    }

    std::cout << "Found " << data->scenes_count << " scenes" << std::endl;
    for (cgltf_size i = 0; i<data->scenes_count; i++) {
        process_scene(data, data->scenes + i);
    }

    return EXIT_SUCCESS;
}
