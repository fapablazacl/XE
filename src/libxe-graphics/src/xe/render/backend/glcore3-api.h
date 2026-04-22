
#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>

#include <xe/render/types.h>
#include <xe/render/RenderBackend.h>

namespace xe {
    struct VertexAttribGL {
        gl::AttribLocation loc;
        GLboolean normalized = GL_FALSE;
        gl::VertexAttribPointerType dataType;
        int dim = 3;

        VertexAttribGL(gl::AttribLocation loc, int dim, gl::VertexAttribPointerType dataType, GLboolean normalized = GL_FALSE) {
            this->loc = loc;
            this->dim = dim;
            this->dataType = dataType;
            this->normalized = normalized;
        }
    };

    struct VertexLayoutGL {
        std::vector<VertexAttribGL> attributes;
        gl::DrawElementsType indexDataType = gl::DrawElementsType::eUnsignedByte;
    };

    struct PipelineGL {
        vec4 clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

        gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;

        //! @note: Consider a Weak ptr
        gl::Program shaderProgram;
    };

    /**
     * @brief A single entry in a backend resource pool.
     *
     * Pairs the RAII holder for the underlying GL object with an 8-bit generation counter.
     * A slot is considered free iff its RAII holder evaluates to false (the GL object has been
     * reset()); no separate liveness flag is needed. The generation is bumped by acquireSlot
     * when the slot is reused, so any stale handle that still references this index will fail
     * the gen() == slot.gen check enforced by the backend.
     *
     * @tparam T the underlying GL resource type (gl::BufferId, gl::Texture, gl::Program, ...)
     */
    template <class T> struct Slot {
        //! RAII-owned GL object. Empty (operator bool -> false) when the slot is free for reuse.
        glaze::Unique<T> obj;

        //! Authoritative generation counter. Bumped by acquireSlot when this slot is reused.
        uint8_t gen = 0;
    };

    /**
     * @brief A pool slot for non-GL resources (e.g. VertexLayoutGL) that live as plain C++ values.
     *
     * Shares the contract used by Slot<T>: the `obj` field evaluates to false iff the slot is free,
     * and the 8-bit generation counter is bumped on reuse so stale handles fail the gen check. Uses
     * std::optional as the liveness wrapper because glaze::Unique is specific to GL handle types.
     *
     * @tparam T the plain (non-GL) resource type held by the pool
     */
    template <class T> struct OptSlot {
        //! Engaged when the slot is live; empty after destroy, awaiting reuse.
        std::optional<T> obj;

        //! Authoritative generation counter. Bumped by acquireSlot when this slot is reused.
        uint8_t gen = 0;
    };

    /**
     * @brief GL-backed implementation of RenderDeviceBackendContext.
     * Holds per-resource pools addressed by Handle::index(). Each pool element is a Slot
     * pairing the RAII-managed GL object with its generation counter; a slot is free when
     * its Slot::obj is empty, and reused in-place by acquireSlot rather than by parallel
     * free lists.
     */
    struct RenderDeviceBackendContextGL : RenderDeviceBackendContext {
        //! Pool of buffer objects. A slot's obj is empty after destroyBufferGL, awaiting reuse.
        std::vector<Slot<gl::BufferId>> buffers;

        //! Pool of linked shader programs. A slot's obj is empty after destroyShaderProgramGL.
        std::vector<Slot<gl::Program>> shaderPrograms;

        //! Pool of texture objects. A slot's obj is empty after destroyTextureGL.
        std::vector<Slot<gl::Texture>> textures;

        //! Pool of vertex layout descriptors. A slot's obj is empty after destroyVertexLayoutGL, awaiting reuse.
        std::vector<OptSlot<VertexLayoutGL>> layouts;
    };

    template<typename ObjectT, typename HandleT>
    std::optional<ObjectT> tryObjectExtract(const std::vector<OptSlot<ObjectT>> &objectsSlot, const HandleT &handle) {
        Slot<ObjectT> const &optSlot = objectsSlot[handle.index()];

        if (optSlot.gen != handle.gen()) {
            return std::nullopt;
        }

        return optSlot.obj;
    }

    tl::expected<RenderDeviceBackendContext *, BackendError> createContextGL();
    void destroyContextGL(RenderDeviceBackendContext *ctx);

    tl::expected<BufferHandle, BackendError> createBufferGL(RenderDeviceBackendContext *ctx, const BufferDescriptor &desc);
    void destroyBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle);
    void readBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle, const BufferReadDescriptor &desc);

    tl::expected<ShaderHandle, BackendError> createShaderProgramGL(RenderDeviceBackendContext *ctx, const ShaderProgramDescriptor &desc);
    void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, ShaderHandle handle);

    tl::expected<TextureHandle, BackendError> createTextureGL(RenderDeviceBackendContext *ctx, const TextureDescriptor &desc);
    void destroyTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle);
    void updateTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle, const TextureUpdateDescriptor &desc);
    void readTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle, const TextureReadDescriptor &desc);

    void initializeBackendTableGL(RenderDeviceBackendVTable *vtable);
} // namespace xe
