
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/render/RenderBackend.h>
#include <cassert>
#include <string>

#include "glcore3-types.h"

namespace xe {
    // -- Fallback stubs --
    static void uniformFvFallback(GLint, GLsizei, const GLfloat *) {
        // TODO: replace assert with a proper logging call once the logging subsystem is in place.
        assert(false && "uniformFvFallback: glUniform*fv function pointer not loaded");
    }

    static void uniformIvFallback(GLint, GLsizei, const GLint *) {
        // TODO: replace assert with a proper logging call once the logging subsystem is in place.
        assert(false && "uniformIvFallback: glUniform*iv function pointer not loaded");
    }

    static void uniformUivFallback(GLint, GLsizei, const GLuint *) {
        // TODO: replace assert with a proper logging call once the logging subsystem is in place.
        assert(false && "uniformUivFallback: glUniform*uiv function pointer not loaded");
    }

    static void uniformMatFvFallback(GLint, GLsizei, GLboolean, const GLfloat *) {
        // TODO: replace assert with a proper logging call once the logging subsystem is in place.
        assert(false && "uniformMatFvFallback: glUniformMatrix*fv function pointer not loaded");
    }

    using UniformFvFn = void (*)(GLint, GLsizei, const GLfloat *);
    using UniformIvFn = void (*)(GLint, GLsizei, const GLint *);
    using UniformUivFn = void (*)(GLint, GLsizei, const GLuint *);
    using UniformMatFvFn = void (*)(GLint, GLsizei, GLboolean, const GLfloat *);

    UniformFvFn s_uniformFv[4] = {uniformFvFallback, uniformFvFallback, uniformFvFallback, uniformFvFallback};
    UniformIvFn s_uniformIv[4] = {uniformIvFallback, uniformIvFallback, uniformIvFallback, uniformIvFallback};
    UniformUivFn s_uniformUiv[4] = {uniformUivFallback, uniformUivFallback, uniformUivFallback, uniformUivFallback};

    UniformMatFvFn s_uniformMatFv[3][3] = {
        {uniformMatFvFallback, uniformMatFvFallback, uniformMatFvFallback},
        {uniformMatFvFallback, uniformMatFvFallback, uniformMatFvFallback},
        {uniformMatFvFallback, uniformMatFvFallback, uniformMatFvFallback},
    };

    /**
     * @brief Dispatch a scalar/vector uniform upload to the correct glUniform{1,2,3,4}{f,i,ui}v call.
     *
     * The backend assumes the payload in `sub.data` is already in the memory layout the matching
     * glUniform*v entrypoint expects: count * getTypeCols(te) tightly-packed elements of
     * the C type implied by getTypeKind(te) (GLfloat / GLint / GLuint).
     */
    static void applyUniformGL(const UniformValueSubmission &sub) {
        GLint const loc = sub.location.raw;
        GLsizei const count = static_cast<GLsizei>(sub.count);
        const TypeEncoding te = static_cast<TypeEncoding>(sub.type);
        const uint8_t cols = getTypeCols(te);
        assert(cols >= 1 && cols <= 4 && "applyUniformGL: unsupported vector width");

        switch (getTypeKind(te)) {
        case TypeKind::Float:
            s_uniformFv[cols - 1](loc, count, static_cast<const GLfloat *>(sub.data));
            return;
        case TypeKind::Int:
            s_uniformIv[cols - 1](loc, count, static_cast<const GLint *>(sub.data));
            return;
        case TypeKind::UInt:
            s_uniformUiv[cols - 1](loc, count, static_cast<const GLuint *>(sub.data));
            return;
        }
        assert(false && "applyUniformGL: unhandled TypeKind");
    }

    /**
     * @brief Dispatch a matrix uniform upload to the correct glUniformMatrix*fv call.
     *
     * Data is always GLfloat; double-precision matrix uniforms are not part of this API surface.
     */
    static void applyUniformMatrixGL(const UniformMatrixSubmission &sub) {
        GLint const loc = sub.location.raw;
        GLsizei const count = static_cast<GLsizei>(sub.count);
        GLboolean const transpose = sub.transpose ? GL_TRUE : GL_FALSE;
        auto const *data = static_cast<const GLfloat *>(sub.data);
        const TypeEncoding te = static_cast<TypeEncoding>(sub.shape);
        const uint8_t cols = getTypeCols(te);
        const uint8_t rows = getTypeRows(te);
        assert(cols >= 2 && cols <= 4 && rows >= 2 && rows <= 4 && "applyUniformMatrixGL: unsupported matrix shape");
        s_uniformMatFv[cols - 2][rows - 2](loc, count, transpose, data);
    }

    tl::expected<UniformLocation, BackendError> resolveUniformLocationGL(RenderDeviceBackendContext *ctx, ShaderHandle handle, const char *name) {
        assert(name != nullptr && "resolveUniformLocationGL: name must not be null");

        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        const ProgramGL *programPtr = tryObjectExtract<ProgramGL>(shaderPrograms, handle);
        if (programPtr == nullptr) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "resolveUniformLocationGL: shader handle is invalid or references a freed program");
        }

        gl::UniformLocation const loc = gl::getUniformLocation(programPtr->program.get(), name);
        if (!loc.valid()) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, std::string{"resolveUniformLocationGL: uniform '"} + name + "' is not active in the shader program");
        }

        UniformLocation out;
        out.raw = loc.loc;
        out.programKey = handle.raw;
        return out;
    }

    void applyUniformsGL(
        RenderDeviceBackendContext *ctx, ShaderHandle handle, const UniformValueSubmission *values, size_t valueCount, const UniformMatrixSubmission *matrices, size_t matrixCount
    ) {
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        const ProgramGL *programPtr = tryObjectExtract<ProgramGL>(shaderPrograms, handle);
        assert(programPtr != nullptr && "applyUniformsGL: shader handle is invalid or references a freed program");

        gl::useProgram(programPtr->program.get());

        for (size_t i = 0; i < valueCount; ++i) {
            assert(values[i].location.isValid() && "applyUniformsGL: UniformValueSubmission carries an invalid location");
            assert(values[i].location.programKey == handle.raw && "applyUniformsGL: UniformValueSubmission location was resolved against a different program");
            assert(values[i].data != nullptr && "applyUniformsGL: UniformValueSubmission::data must not be null");
            applyUniformGL(values[i]);
        }

        for (size_t i = 0; i < matrixCount; ++i) {
            assert(matrices[i].location.isValid() && "applyUniformsGL: UniformMatrixSubmission carries an invalid location");
            assert(matrices[i].location.programKey == handle.raw && "applyUniformsGL: UniformMatrixSubmission location was resolved against a different program");
            assert(matrices[i].data != nullptr && "applyUniformsGL: UniformMatrixSubmission::data must not be null");
            applyUniformMatrixGL(matrices[i]);
        }
    }

    void bindUniformBufferGL(RenderDeviceBackendContext *ctx, uint32_t bindingPoint, BufferHandle handle, size_t offset, size_t size) {
        assert(handle.subType() == BufferType::Uniform && "bindUniformBufferGL: handle must reference a BufferType::Uniform buffer");

        auto &buffers = glctx(ctx)->buffers;
        uint32_t const index = handle.index();
        assert(index < buffers.size() && "bindUniformBufferGL: handle index out of range");
        auto &slot = buffers[index];
        assert(slot.obj && "bindUniformBufferGL: use of freed handle");
        assert(slot.gen == handle.gen() && "bindUniformBufferGL: stale handle (generation mismatch)");

        if (size == 0) {
            gl::bindBufferBase(gl::BufferTarget::eUniformBuffer, bindingPoint, slot.obj->buffer);
        } else {
            gl::bindBufferRange(gl::BufferTarget::eUniformBuffer, bindingPoint, slot.obj->buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
        }
    }

    void setUniform(const UniformValueSubmission *uniform, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            applyUniformGL(uniform[i]);
        }
    }

    void setUniformMatrix(const UniformMatrixSubmission *uniformMatrix, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            applyUniformMatrixGL(uniformMatrix[i]);
        }
    }
} // namespace xe
