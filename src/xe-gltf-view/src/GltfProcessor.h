
#pragma once

#include <iostream>

#include "GltfUtil.h"
#include "cgltf.h"
#include "xe/gl/RendererGL.h"

namespace xe::gl {
    class RendererGL;
}

class GltfProcessor {
public:
    bool process(const std::string &filePath) {
        std::cout << "Loading " << filePath << '\n';

        cgltf_options const options = {};
        cgltf_data *data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &data);

        if (result != cgltf_result_success) {
            std::cerr << "CGLTF: Couldn't load file '" << filePath << "'. Error code: " << to_string(result);
            return false;
        }

        result = cgltf_load_buffers(&options, data, filePath.c_str());
        if (result != cgltf_result_success) {
            std::cerr << "CGLTF: error while loading buffers '" << filePath << "'. Error code: " << to_string(result);
            return false;
        }

        std::cout << "Found " << data->lights_count << " lights" << '\n';
        for (cgltf_size i = 0; i < data->lights_count; i++) {
            process_light(data->lights + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->cameras_count << " cameras" << '\n';
        for (cgltf_size i = 0; i < data->cameras_count; i++) {
            process_camera(data->cameras + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->meshes_count << " meshes" << '\n';
        for (cgltf_size i = 0; i < data->meshes_count; i++) {
            process_mesh(data->meshes + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->animations_count << " animations" << '\n';
        for (cgltf_size i = 0; i < data->animations_count; i++) {
            process_animation(data->animations + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->skins_count << " skins" << '\n';
        for (cgltf_size i = 0; i < data->skins_count; i++) {
            process_skins(data->skins + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->textures_count << " textures" << '\n';
        for (cgltf_size i = 0; i < data->textures_count; i++) {
            process_texture(data->textures + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->images_count << " images" << '\n';
        for (cgltf_size i = 0; i < data->images_count; i++) {
            process_image(data->images + i);
            std::cout << '\n';
        }
        std::cout << '\n';

        std::cout << "Found " << data->samplers_count << " samplers" << '\n';
        for (cgltf_size i = 0; i < data->samplers_count; i++) {
            process_sampler(data->samplers + i);
        }
        std::cout << '\n';

        std::cout << "Found " << data->accessors_count << " accessors" << '\n';
        // for (cgltf_size i = 0; i < data->accessors_count; i++) {
        //     process_accessor(data->accessors + i);
        // }
        std::cout << '\n';

        std::cout << "Found " << data->buffers_count << " buffers" << '\n';
        for (cgltf_size i = 0; i < data->buffers_count; i++) {
            process_buffer(data->buffers + i);
        }
        std::cout << '\n';

        std::cout << "Found " << data->buffer_views_count << " buffer views" << '\n';
        for (cgltf_size i = 0; i < data->buffer_views_count; i++) {
            process_buffer_view(data->buffer_views + i);
        }
        std::cout << '\n';

        std::cout << "Found " << data->data_extensions_count << " data extensions" << '\n';
        for (cgltf_size i = 0; i < data->data_extensions_count; i++) {
            process_data_extension(data->data_extensions + i);
        }
        std::cout << '\n';

        std::cout << "Found " << data->extensions_required_count << " extensions required" << '\n';
        for (cgltf_size i = 0; i < data->extensions_required_count; i++) {
            process_extensions_required(data->extensions_required[i]);
        }
        std::cout << '\n';

        std::cout << "Found " << data->extensions_used_count << " extensions used" << '\n';
        for (cgltf_size i = 0; i < data->extensions_used_count; i++) {
            process_extensions_used(data->extensions_used[i]);
        }
        std::cout << '\n';

        std::cout << "Found " << data->variants_count << " variants" << '\n';
        for (cgltf_size i = 0; i < data->variants_count; i++) {
            process_variant(data->variants + i);
        }
        std::cout << '\n';

        std::cout << "Found " << data->scenes_count << " scenes" << '\n';
        for (cgltf_size i = 0; i < data->scenes_count; i++) {
            process_scene(data, data->scenes + i);
        }
        std::cout << '\n';

        cgltf_free(data);

        return true;
    }

private:
    static void process_camera(cgltf_camera *camera) {
        std::cout << "Camera node name: " << camera->name << '\n';
    }

    static void process_light(cgltf_light *light) {
        std::cout << "Light node name: " << light->name << '\n';
    }

    static void process_material(const cgltf_material *material) {
        std::cout << "Material name " << material->name << '\n';

        std::cout << "Material has clearcoat " << evaluate_bool(material->has_clearcoat) << '\n';
        std::cout << "Material has ior " << evaluate_bool(material->has_ior) << '\n';
        std::cout << "Material has metallic roughness " << evaluate_bool(material->has_pbr_metallic_roughness) << '\n';
        std::cout << "Material has specular glossiness " << evaluate_bool(material->has_pbr_specular_glossiness) << '\n';
        std::cout << "Material has sheen " << evaluate_bool(material->has_sheen) << '\n';
        std::cout << "Material has specular " << evaluate_bool(material->has_specular) << '\n';
        std::cout << "Material has transmission " << evaluate_bool(material->has_transmission) << '\n';
        std::cout << "Material has volume " << evaluate_bool(material->has_volume) << '\n';
    }

    static void process_buffer_view(const cgltf_buffer_view *view) {
        std::cout << "Buffer view name: " << ((view->name != nullptr) ? view->name : "<noname>") << '\n';
        std::cout << "Buffer view type: " << view->type << '\n';
    }

    void process_accessor(const cgltf_accessor *accessor) {
        std::cout << "Accessor name: " << sanitizeString(accessor->name) << '\n';
        std::cout << "Accessor count: " << accessor->count << '\n';
        std::cout << "Accessor type: " << accessor->type << '\n';
        std::cout << "Accessor component type: " << accessor->component_type << '\n';
        std::cout << "Accessor extensions count: " << accessor->extensions_count << '\n';
        std::cout << "Accessor have buffer_view: " << toBool(accessor->buffer_view) << '\n';
        std::cout << "Accessor has max: " << toBool(accessor->has_max) << '\n';
        std::cout << "Accessor has min: " << toBool(accessor->has_min) << '\n';
        std::cout << "Accessor is sparse: " << toBool(accessor->is_sparse) << '\n';
        std::cout << "Accessor normalized: " << toBool(accessor->normalized) << '\n';

        if (accessor->buffer_view != nullptr) {
            process_buffer_view(accessor->buffer_view);
        }
    }

    void process_attribute(const cgltf_attribute &attribute) {
        std::cout << "Attribute name " << attribute.name << '\n';
        std::cout << "Attribute index " << attribute.index << '\n';
        std::cout << "Attribute type " << attribute.type << '\n';
        process_accessor(attribute.data);
    }

    // process a single mesh primitive
    void process_primitive(const cgltf_primitive &primitive) {
        std::cout << "Primitive type " << primitive.type << '\n';
        std::cout << "Primitive attribute count  " << primitive.attributes_count << '\n';
        std::cout << "Primitive has indices " << toBool(primitive.indices) << '\n';
        std::cout << "Primitive has material " << toBool(primitive.material) << '\n';
        std::cout << "Primitive mappings count " << primitive.mappings_count << '\n';
        std::cout << "Primitive targets count " << primitive.targets_count << '\n';
        std::cout << "Primitive has draco mesh compression " << toBool(primitive.has_draco_mesh_compression) << '\n';
        std::cout << "Primitive extension count " << primitive.extensions_count << '\n';

        if (primitive.indices != nullptr) {
            auto *const indexAccessor = primitive.indices;
            const auto indicesOffset = indexAccessor->buffer_view->offset;
            const auto indicesSize = indexAccessor->buffer_view->size;

            std::cout << "Primitive indices offset " << indicesOffset << '\n';
            std::cout << "Primitive indices size " << indicesSize << '\n';
            // process_accessor(primitive.indices);
        }

        /*
        for (cgltf_size i = 0; i < primitive.attributes_count; i++) {
            const auto attribute = primitive.attributes[i];
            const cgltf_accessor *accessor = attribute.data;

            std::cout << "Attribute name " << attribute.name << std::endl;
            std::cout << "Attribute index " << attribute.index << std::endl;
            std::cout << "Attribute type " << attribute.type << std::endl;

            std::cout << "Attribute accessor address: " << accessor << std::endl;
            std::cout << "Attribute accessor name: " << toStr(accessor->name) << std::endl;
            std::cout << "Attribute accessor count: " << accessor->count << std::endl;
            std::cout << "Attribute accessor type: " << accessor->type << std::endl;
            std::cout << "Attribute accessor component type: " << accessor->component_type << std::endl;
            std::cout << "Attribute accessor extensions count: " << accessor->extensions_count << std::endl;
            std::cout << "Attribute accessor have buffer_view: " << toBool(accessor->buffer_view) << std::endl;
            std::cout << "Attribute accessor has max: " << toBool(accessor->has_max) << std::endl;
            std::cout << "Attribute accessor has min: " << toBool(accessor->has_min) << std::endl;
            std::cout << "Attribute accessor is sparse: " << toBool(accessor->is_sparse) << std::endl;
            std::cout << "Attribute accessor normalized: " << toBool(accessor->normalized) << std::endl;

            if (accessor->buffer_view) {
                const auto view = accessor->buffer_view;

                std::cout << "Buffer view address: " << view << std::endl;
                std::cout << "Buffer view name: " << toStr(view->name) << std::endl;
                std::cout << "Buffer view type: " << view->type << std::endl;
                std::cout << "Buffer view offset, size: " << view->offset << ", " << view->size << std::endl;
                std::cout << "Buffer view stride: " << view->stride << std::endl;
            }
        }
        */

        if (primitive.material != nullptr) {
            process_material(primitive.material);
        }

        for (cgltf_size i = 0; i < primitive.targets_count; i++) {
            auto *target = primitive.targets + i;
            std::cout << "Primitive morph target " << i << " attribute count: " << target->attributes_count << '\n';

            for (cgltf_size j = 0; j < target->attributes_count; j++) {
                std::cout << "Primitive morph target " << i << " attribute " << j << " name: " << target->attributes[j].name << '\n';
            }
        }
    }

    void process_mesh(cgltf_mesh *mesh) {
        std::cout << "Node mesh " << mesh->name << '\n';
        std::cout << "Node primitives count " << mesh->primitives_count << '\n';

        for (cgltf_size i = 0; i < mesh->primitives_count; i++) {
            process_primitive(mesh->primitives[i]);
        }
    }

    void process_node(cgltf_node *node) {
        std::cout << "Node name" << node->name << '\n';
        std::cout << "Node has matrix " << node->has_matrix << '\n';
        std::cout << "Node has translation " << node->has_translation << '\n';
        std::cout << "Node has rotation " << node->has_rotation << '\n';
        std::cout << "Node has scale " << node->has_scale << '\n';

        if (node->mesh != nullptr) {
            process_mesh(node->mesh);
        }

        if (node->light != nullptr) {
            process_light(node->light);
        }

        if (node->camera != nullptr) {
            process_camera(node->camera);
        }

        std::cout << "Node children " << node->children_count << '\n';
        for (cgltf_size ci = 0; ci < node->children_count; ci++) {
            process_node(node->children[ci]);
        }
    }

    void process_scene(cgltf_data *data, cgltf_scene *scene) {
        std::cout << "Scene name " << scene->name << '\n';
        std::cout << "Scene node count " << scene->nodes_count << '\n';

        for (cgltf_size ni = 0; ni < scene->nodes_count; ni++) {
            process_node(data->nodes + ni);
        }
    }

    static void process_animation(cgltf_animation *animation) {
        std::cout << "Animation name: " << evaluate_name(animation->name) << '\n';
        std::cout << "Animation samplers count: " << animation->samplers_count << '\n';
        std::cout << "Animation channels count: " << animation->channels_count << '\n';
        std::cout << "Animation extensions count: " << animation->extensions_count << '\n';
    }

    static void process_skins(cgltf_skin *skin) {
        std::cout << "Skin name: " << evaluate_name(skin->name) << '\n';
        std::cout << evaluate_ptr("Skin has skeleton node: ", skin->skeleton) << '\n';
        std::cout << "Animation joints count: " << skin->joints_count << '\n';
        std::cout << "Animation extensions count: " << skin->extensions_count << '\n';
    }

    static void process_texture(cgltf_texture *texture) {
        std::cout << "Texture name: " << ((texture->name != nullptr) ? texture->name : "<noname>") << '\n';
        std::cout << "Texture extensions count: " << texture->extensions_count << '\n';
        std::cout << "Texture has sampler: " << ((texture->sampler != nullptr) ? "true" : "false") << '\n';

        const cgltf_sampler *sampler = texture->sampler;

        if (sampler != nullptr) {
            std::cout << "Texture sampler name: " << ((sampler->name != nullptr) ? sampler->name : "<noname>") << '\n';
            std::cout << "Texture mag filter: " << sampler->mag_filter << '\n';
            std::cout << "Texture min filter: " << sampler->min_filter << '\n';
            std::cout << "Texture wrap s: " << sampler->wrap_s << '\n';
            std::cout << "Texture wrap t: " << sampler->wrap_t << '\n';
            std::cout << "Texture extensions count: " << sampler->extensions_count << '\n';
        }

        std::cout << "Texture has basisu: " << ((texture->has_basisu != 0) ? "true" : "false") << '\n';
    }

    static void process_image(cgltf_image *image) {
        std::cout << "Image name: " << ((image->name != nullptr) ? image->name : "<noname>") << '\n';
        std::cout << "Image uri: " << ((image->uri != nullptr) ? image->uri : "<noname>") << '\n';
        std::cout << "Image mime type: " << ((image->mime_type != nullptr) ? image->mime_type : "<noname>") << '\n';
        std::cout << "Image has buffer view: " << ((image->buffer_view != nullptr) ? "true" : "false") << '\n';
        std::cout << "Image extensions count: " << image->extensions_count << '\n';
    }

    static void process_sampler(cgltf_sampler *sampler) {
        std::cout << "Image name: " << ((sampler->name != nullptr) ? sampler->name : "<noname>") << '\n';
    }

    static void process_buffer(cgltf_buffer *buffer) {
        std::cout << "Buffer name: " << sanitizeString(buffer->name) << '\n';
        std::cout << "Buffer uri: " << sanitizeString(buffer->uri) << '\n';
        std::cout << "Buffer size: " << (static_cast<float>(buffer->size) / 1024.0F / 1024.0F) << " MB" << '\n';
    }

    static void process_data_extension(cgltf_extension *extension) {
        std::cout << "Data extension name" << extension->name << '\n';
    }

    static void process_extensions_required(char *str) {
        std::cout << "Extension required " << str << '\n';
    }

    static void process_extensions_used(char *str) {
        std::cout << "Extension used " << str << '\n';
    }

    static void process_variant(cgltf_material_variant *material_variant) {
        std::cout << "Material variant name: " << material_variant->name << '\n';
    }
};
