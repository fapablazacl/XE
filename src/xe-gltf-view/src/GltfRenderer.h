
#pragma once

#include <memory>
#include "xe/gl/RendererGL.h"
#include "GltfMesh.h"

namespace xe::gltf_view {
    struct ShaderProgramUniformData {
        XE::Matrix4 projViewModel = XE::mat4Identity();

        XE::Matrix4 model = XE::mat4Identity();

        // The Texture Unit which contains the diffuse texture
        GLint diffuseTexture = 0;

        float seconds = 0.0f;

        [[nodiscard]]
        std::vector<xe::gl::UniformMatrix> mapMatrixUniforms(xe::gl::Program shaderProgram) const;

        [[nodiscard]]
        std::vector<xe::gl::Uniform> mapUniforms(xe::gl::Program shaderProgram) const;
    };

    class GltfRenderer {
    public:
        explicit GltfRenderer();

        ~GltfRenderer();

        void render();

        void beginFrame();

        void endFrame();

    private:
        std::unique_ptr<xe::gl::RendererGL> renderer;
        ShaderProgramUniformData uniformData;
        float startSeconds = 0.0f;
        float angle = 0.0f;
        xe::gl::Program program;
        std::vector<GltfMesh> meshes;
    };
}
