
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
        gl::DrawElementsType indexDataType = gl::DrawElementsType::eUnsignedShort;
    };

    struct ProgramGL {
        glaze::Unique<gl::Program> program;
    };

    struct BufferGL {
        glaze::Unique<gl::BufferId> buffer;
    };

    struct TextureGL {
        glaze::Unique<gl::Texture> texture;
    };

    /**
     * @brief A single geometry: the VAO wiring together buffer bindings, plus the draw-time index
     * data type captured at creation so glDrawElements calls don't need to re-query the layout.
     */
    struct GeometryGL {
        //! RAII-owned VAO that encodes the attribute bindings and the element array buffer binding.
        glaze::Unique<gl::VertexArray> vao;

        //! Index component type (uint16 or uint32), propagated from the source VertexLayoutGL.
        gl::DrawElementsType indexDataType = gl::DrawElementsType::eUnsignedShort;
    };

    /**
     * @brief A resolved uniform block binding baked into a PipelineGL at creation time.
     *
     * createPipelineGL calls glGetUniformBlockIndex followed by glUniformBlockBinding for every
     * entry, so the GPU-side wiring is already in place by the time a pipeline is used. Kept on
     * the pipeline for diagnostics (allowing a debugger to inspect which binding points are
     * actually live) and so destroyPipeline has the information to tear down any future state
     * caches keyed on the pipeline.
     */
    struct UniformBlockBindingGL {
        //! Block name resolved against the shader program at pipeline-creation time.
        std::string blockName;

        //! Binding point assigned to this block (matches PipelineUniformBlock::bindingPoint).
        uint32_t bindingPoint = 0;

        //! Block index returned by glGetUniformBlockIndex, captured for diagnostics.
        uint32_t blockIndex = 0;
    };

    struct SamplerGL {
        gl::Sampler sampler;
    };

    /**
     * @brief GL-side representation of a PipelineDescriptor.
     *
     * Holds the clear state and a non-owning reference to the linked program, plus the uniform
     * block bindings that were baked into the program at pipeline creation. The pipeline itself
     * does not own the program - ShaderHandle ownership stays with the shader pool.
     */
    struct PipelineGL {
        //! RGBA clear color applied at beginFrame (once beginFrame is wired up on this backend).
        vec4 clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

        //! Mask selecting which buffers glClear will clear.
        gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;

        //! Raw GL id for the program this pipeline draws with. Not owning.
        gl::Program shaderProgram;

        //! Original shader handle, retained for debug validation of uniform-block rebinds.
        ShaderHandle shaderHandle;

        //! Uniform block bindings baked into the program by createPipelineGL.
        std::vector<UniformBlockBindingGL> uniformBlockBindings;
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
        std::vector<OptSlot<BufferGL>> buffers;

        //! Pool of linked shader programs. A slot's obj is empty after destroyShaderProgramGL.
        std::vector<OptSlot<ProgramGL>> shaderPrograms;

        //! Pool of texture objects. A slot's obj is empty after destroyTextureGL.
        std::vector<OptSlot<TextureGL>> textures;

        //! Pool of vertex layout descriptors. A slot's obj is empty after destroyVertexLayoutGL, awaiting reuse.
        std::vector<OptSlot<VertexLayoutGL>> layouts;

        //! Pool of geometries (VAO + index metadata). A slot's obj is empty after destroyGeometryGL, awaiting reuse.
        std::vector<OptSlot<GeometryGL>> geometries;

        //! Pool of pipeline objects (baked clear state + shader program reference + uniform block bindings).
        std::vector<OptSlot<PipelineGL>> pipelines;
    };

    /**
     * @brief Validate a handle against an OptSlot pool and return a pointer to the held object, or nullptr.
     *
     * Performs the full three-step handle check (index in range, slot live, generation matches). Returns
     * a borrowed pointer into the pool so callers avoid copying the underlying resource (e.g. a
     * VertexLayoutGL's attribute vector). The pointer is valid until the pool is mutated by the next
     * acquireSlot / destroy call on this pool.
     *
     * @tparam ObjectT the plain resource type held by the pool
     * @tparam HandleT the typed handle kind (must expose index() and gen())
     * @param pool the OptSlot-backed pool to look up in
     * @param handle the handle to validate
     * @return const pointer to the held object on success; nullptr if any check fails
     */
    template <typename ObjectT, typename HandleT> const ObjectT *tryObjectExtract(const std::vector<OptSlot<ObjectT>> &pool, const HandleT &handle) {
        uint32_t const index = handle.index();
        if (index >= pool.size()) {
            return nullptr;
        }

        OptSlot<ObjectT> const &slot = pool[index];
        if (!slot.obj) {
            return nullptr;
        }
        if (slot.gen != handle.gen()) {
            return nullptr;
        }

        return &*slot.obj;
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

    tl::expected<PipelineHandle, BackendError> createPipelineGL(RenderDeviceBackendContext *ctx, const PipelineDescriptor &desc);
    void destroyPipelineGL(RenderDeviceBackendContext *ctx, PipelineHandle handle);

    tl::expected<UniformLocation, BackendError> resolveUniformLocationGL(RenderDeviceBackendContext *ctx, ShaderHandle handle, const char *name);
    void applyUniformsGL(
        RenderDeviceBackendContext *ctx, ShaderHandle handle, const UniformValueSubmission *values, size_t valueCount, const UniformMatrixSubmission *matrices, size_t matrixCount
    );
    void bindUniformBufferGL(RenderDeviceBackendContext *ctx, uint32_t bindingPoint, BufferHandle handle, size_t offset, size_t size);

    void initializeBackendTableGL(RenderDeviceBackendVTable *vtable);
} // namespace xe
