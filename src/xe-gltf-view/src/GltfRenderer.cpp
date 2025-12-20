
#include "GltfRenderer.h"

#include <array>
#include <filesystem>
#include <stdexcept>
#include <span>

#include "fmt/printf.h"
#include "xe/gl/RendererGL.h"
#include "xe/Logger.h"
#include "xe/FileUtil.h"

#include "GltfDataLoader.h"
#include "GltfProcessor.h"
#include "xe/ImageLoader.h"
#include "xe/Timer.h"
#include "xe/math/Matrix.h"
#include "xe/math/Quaternion.h"
#include "xe/math/Vector.h"

namespace xe::gltf_view {

    template <typename T> const T SCREEN_WIDTH = T{640};

    template <typename T> const T SCREEN_HEIGHT = T{480};

    std::vector<xe::gl::UniformMatrix> ShaderProgramUniformData::mapMatrixUniforms(xe::gl::Program shaderProgram) const {
        return {
            xe::gl::makeUniform(shaderProgram.getUniformLocation("modelViewProj"), projViewModel),
        };
    }

    [[nodiscard]]
    std::vector<xe::gl::Uniform> ShaderProgramUniformData::mapUniforms(xe::gl::Program shaderProgram) const {
        return {
            xe::gl::makeUniform(shaderProgram.getUniformLocation("diffuseTexture"), diffuseTexture),
            xe::gl::makeUniform(shaderProgram.getUniformLocation("seconds"), seconds),
        };
    }

    GltfRenderer::GltfRenderer(const std::string &filePath) {
        renderer = xe::gl::RendererGL::create();

        const std::filesystem::path internalAssetsPath = XE_GLTF_VIEW_SOURCE_FOLDER;
        const std::string vertexShaderSource = XE::loadTextFile(internalAssetsPath / "shaders/gltf-view.vert");
        const std::string fragmentShaderSource = XE::loadTextFile(internalAssetsPath / "shaders/gltf-view.frag");

        std::vector<xe::gl::Shader> shaders = {
            renderer->createShader(GL_VERTEX_SHADER, vertexShaderSource.c_str()),
            renderer->createShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str())
        };

        program = renderer->createProgram(shaders);
        if (!program.id) {
            throw std::runtime_error("Failed create program.");
        }

        auto gltfParser = GltfDataParser{};
        auto gltfData = gltfParser.parse(filePath);

        if (!gltfData) {
            throw std::runtime_error("Failed gltfParser.parse().");
        }

        data = gltfData;

        auto imageLoader = createImageLoader();
        auto gltfTextureLoader = GltfTextureLoader{renderer.get(), imageLoader.get()};

        auto gltfLoader = GltfDataLoader{
            gltfData,
            renderer.get(),
            &gltfTextureLoader,
            program,
            {{"POSITION", ShaderAttrib("vertCoord")}, {"NORMAL", ShaderAttrib{"vertNormal"}}, {"TEXCOORD_0", ShaderAttrib{"vertTexCoord"}}}
        };

        gltfLoader.loadAllAnimations();

        meshes = gltfLoader.loadAllMeshes();

        if (meshes.empty()) {
            throw std::runtime_error("Meshes could not be loaded.");
        }

        std::cout << meshes.size() << " meshes were loaded" << '\n';
    }

    GltfRenderer::~GltfRenderer() {
    }

    void GltfRenderer::beginFrame() {
        const auto clearFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
        const auto clearColor = XE::Vector4{0.2f, 0.2f, 0.8f, 1.0f};

        renderer->clear(clearFlags, clearColor, {}, {});
    }

    void GltfRenderer::endFrame() {
        renderer->flush();
    }

    void GltfRenderer::render() {
        uniformData.seconds = (static_cast<float>(XE::Timer::getTick()) / 1000.0f) - startSeconds;
        startSeconds = uniformData.seconds;

        const float aspect = SCREEN_HEIGHT<float> / SCREEN_WIDTH<float>;
        const auto proj = XE::mat4Perspective(XE::radians(60.0f), aspect, 0.001f, 1000.0f);

        const XE::Vector3 eye = {0.0f, 0.0f, -25.0f};
        const XE::Vector3 at = {0.0f, 0.0f, 0.0f};
        const XE::Vector3 up = {0.0f, 1.0f, 0.0f};
        const auto view = XE::mat4LookAtRH(eye, at, up);
        const auto model = XE::mat4RotationY(angle += 0.005f);

        uniformData.model = model;
        uniformData.projViewModel = proj * view * model;

        const std::vector<xe::gl::CapabilityStatus> renderState = {{GL_DEPTH_TEST, GL_TRUE}, {GL_CULL_FACE, GL_TRUE}};

        renderer->bindRenderState(renderState);
        renderer->useProgram(program);

        renderer->bindRenderState(uniformData.mapUniforms(program));
        renderer->bindRenderState(uniformData.mapMatrixUniforms(program));

        for (const auto &mesh : meshes) {
            for (const auto &meshSubset : mesh.primitives) {
                const xe::gl::VertexArrayPrimitive prims[] = {{0, meshSubset.count}};

                xe::gl::TextureLayer layer;
                layer.texture = meshSubset.material.texture;
                renderer->bindRenderState({&layer, 1});

                if (meshSubset.indexData.has_value()) {
                    renderer->draw(meshSubset.vao, meshSubset.primitive, prims, meshSubset.indexData->type);
                } else {
                    renderer->draw(meshSubset.vao, meshSubset.primitive, prims);
                }
            }
        }
    }

    XE::Matrix4 computeLocalTransformation(const cgltf_node &node) {
        if (node.has_matrix) {
            return XE::Matrix4(node.matrix);
        }

        if (node.has_rotation) {
            XE_LOG_ERROR("Node rotation is not supported. Defaulting to Identity");
            // transform = XE::Quat(node.rotation);
            return XE::mat4Identity();
        }

        if (node.has_scale) {
            return XE::mat4Scaling(XE::Vector4(XE::Vector3(node.scale), 1.0f));
        }

        if (node.has_translation) {
            return XE::mat4Translation(XE::Vector3(node.translation));
        }

        return XE::mat4Identity();
    }

    void GltfRenderer::renderScene(const cgltf_scene &scene) {
        std::span<cgltf_node *> nodes{scene.nodes, scene.nodes_count};

        for (const cgltf_node *node : nodes) {
            if (node) {
                const XE::Matrix4 transformation = XE::mat4Identity();
                renderNode(transformation, *node);
            }
        }
    }

    void GltfRenderer::renderNode(const XE::Matrix4 &parentTransformation, const cgltf_node &node) {
        const XE::Matrix4 transformation = parentTransformation * computeLocalTransformation(node);

        if (node.camera) {
            renderCamera(*node.camera);
        }

        if (node.mesh) {
            renderMesh(*node.mesh);
        }

        std::span<cgltf_node *> children{node.children, node.children_count};

        for (const cgltf_node *child : children) {
            if (child) {
                renderNode(transformation, *child);
            }
        }
    }

    void GltfRenderer::renderCamera(const cgltf_camera &camera) {
        switch (camera.type) {
        case cgltf_camera_type_perspective: {
            const cgltf_camera_perspective &pers = camera.data.perspective;
            projection = XE::mat4Perspective(pers.yfov, pers.aspect_ratio, pers.znear, pers.zfar);
            break;
        }

        case cgltf_camera_type_orthographic: {
            XE_LOG_ERROR("Orthographic camera is not supported.");
            break;
        }
        }
    }

    void GltfRenderer::renderMesh(const cgltf_mesh &cgltfMesh) {
        const auto it = meshMap.find(&cgltfMesh);

        if (it == meshMap.end()) {
            XE_LOG_WARNING("Mesh not found in meshMap.");
            return;
        }

        const GltfMesh &mesh = it->second;

        for (const auto &meshSubset : mesh.primitives) {
            const xe::gl::VertexArrayPrimitive prims[] = {{0, meshSubset.count}};

            xe::gl::TextureLayer layer;
            layer.texture = meshSubset.material.texture;
            renderer->bindRenderState({&layer, 1});

            if (meshSubset.indexData.has_value()) {
                renderer->draw(meshSubset.vao, meshSubset.primitive, prims, meshSubset.indexData->type);
            } else {
                renderer->draw(meshSubset.vao, meshSubset.primitive, prims);
            }
        }
    }
} // namespace xe::gltf_view
