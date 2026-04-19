
#pragma once

#include <vector>

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>

#include <xe/render/types.h>
#include <xe/render/RenderBackend.h>

namespace xe {
    /**
     * @brief GL-backed implementation of RenderDeviceBackendContext.
     * Holds per-resource pools addressed by Handle::index(). Slots are kept
     * RAII-managed internally via glaze::Unique while the public API exposed
     * by this header is a flat, non-RAII set of free functions over this
     * context.
     */
    struct RenderDeviceBackendContextGL : RenderDeviceBackendContext {
        //! Pool of buffer objects. Slot may be empty after destroyBufferGL.
        std::vector<glaze::Unique<gl::BufferId>> buffers;

        //! Pool of linked shader programs. Slot may be empty after destroyShaderProgramGL.
        std::vector<glaze::Unique<gl::Program>> shaderPrograms;

        //! Pool of texture objects. Slot may be empty after destroyTextureGL.
        std::vector<glaze::Unique<gl::Texture>> textures;
    };

    RenderDeviceBackendContext* createContextGL();
    void destroyContextGL(RenderDeviceBackendContext* ctx);

    Handle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc);
    void destroyBufferGL(RenderDeviceBackendContext* ctx, Handle handle);
    void readBufferGL(RenderDeviceBackendContext* ctx, Handle handle, const BufferReadDescriptor& desc);

    Handle createShaderProgramGL(RenderDeviceBackendContext* ctx, const ShaderProgramDescriptor& desc);
    void destroyShaderProgramGL(RenderDeviceBackendContext* ctx, Handle handle);

    Handle createTextureGL(RenderDeviceBackendContext* ctx, const TextureDescriptor& desc);
    void destroyTextureGL(RenderDeviceBackendContext* ctx, Handle handle);
    void updateTextureGL(RenderDeviceBackendContext* ctx, Handle handle, const TextureUpdateDescriptor& desc);
    void readTextureGL(RenderDeviceBackendContext* ctx, Handle handle, const TextureReadDescriptor& desc);

    void initializeBackendTableGL(RenderDeviceBackendVTable* vtable);
}
