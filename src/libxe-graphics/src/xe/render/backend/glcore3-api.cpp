
#include <xe/render/RenderBackend.h>
#include <new>

#include "glcore3-types.h"

namespace xe {
    tl::expected<RenderDeviceBackendContext *, BackendError> createContextGL() {
        auto *ctx = new (std::nothrow) RenderDeviceBackendContextGL();
        if (!ctx) {
            return makeBackendError(BackendErrorCode::AllocationFailed, "failed to allocate RenderDeviceBackendContextGL");
        }
        return ctx;
    }

    void destroyContextGL(RenderDeviceBackendContext *ctx) {
        delete glctx(ctx);
    }

    // TODO: Put these into a common definition header
    using UniformFvFn = void (*)(GLint, GLsizei, const GLfloat *);
    using UniformIvFn = void (*)(GLint, GLsizei, const GLint *);
    using UniformUivFn = void (*)(GLint, GLsizei, const GLuint *);
    using UniformMatFvFn = void (*)(GLint, GLsizei, GLboolean, const GLfloat *);

    extern UniformFvFn s_uniformFv[4];
    extern UniformIvFn s_uniformIv[4];
    extern UniformUivFn s_uniformUiv[4];
    extern UniformMatFvFn s_uniformMatFv[3][3];

    void initializeBackendTableGL(RenderDeviceBackendVTable *vtable) {
        vtable->createContext = &createContextGL;
        vtable->destroyContext = &destroyContextGL;
        vtable->createBuffer = &createBufferGL;
        vtable->destroyBuffer = &destroyBufferGL;
        vtable->readBuffer = &readBufferGL;
        vtable->createShaderProgram = &createShaderProgramGL;
        vtable->destroyShaderProgram = &destroyShaderProgramGL;
        vtable->createTexture = &createTextureGL;
        vtable->destroyTexture = &destroyTextureGL;
        vtable->updateTexture = &updateTextureGL;
        vtable->readTexture = &readTextureGL;
        vtable->createVertexLayout = &createVertexLayoutGL;
        vtable->destroyVertexLayout = &destroyVertexLayoutGL;
        vtable->createGeometry = &createGeometryGL;
        vtable->destroyGeometry = &destroyGeometryGL;
        vtable->createPipeline = &createPipelineGL;
        vtable->destroyPipeline = &destroyPipelineGL;
        vtable->resolveUniformLocation = &resolveUniformLocationGL;
        vtable->applyUniforms = &applyUniformsGL;
        vtable->bindUniformBuffer = &bindUniformBufferGL;
        vtable->submitCommand = &submitCommandGL;

        s_uniformFv[0] = glUniform1fv;
        s_uniformFv[1] = glUniform2fv;
        s_uniformFv[2] = glUniform3fv;
        s_uniformFv[3] = glUniform4fv;

        s_uniformIv[0] = glUniform1iv;
        s_uniformIv[1] = glUniform2iv;
        s_uniformIv[2] = glUniform3iv;
        s_uniformIv[3] = glUniform4iv;

        s_uniformUiv[0] = glUniform1uiv;
        s_uniformUiv[1] = glUniform2uiv;
        s_uniformUiv[2] = glUniform3uiv;
        s_uniformUiv[3] = glUniform4uiv;

        s_uniformMatFv[0][0] = glUniformMatrix2fv;   // 2c 2r
        s_uniformMatFv[0][1] = glUniformMatrix2x3fv; // 2c 3r
        s_uniformMatFv[0][2] = glUniformMatrix2x4fv; // 2c 4r
        s_uniformMatFv[1][0] = glUniformMatrix3x2fv; // 3c 2r
        s_uniformMatFv[1][1] = glUniformMatrix3fv;   // 3c 3r
        s_uniformMatFv[1][2] = glUniformMatrix3x4fv; // 3c 4r
        s_uniformMatFv[2][0] = glUniformMatrix4x2fv; // 4c 2r
        s_uniformMatFv[2][1] = glUniformMatrix4x3fv; // 4c 3r
        s_uniformMatFv[2][2] = glUniformMatrix4fv;   // 4c 4r
    }
} // namespace xe
