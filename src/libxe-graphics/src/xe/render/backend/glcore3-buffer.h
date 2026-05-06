
#pragma once

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>

#include <xe/render/types.h>
#include <xe/render/RenderBackend.h>
#include "glcore3-common.h"

namespace xe {

    struct BufferGL {
        glaze::Unique<gl::BufferId> buffer;
    };

    tl::expected<BufferHandle, BackendError> createBufferGL(RenderDeviceBackendContext *ctx, const BufferDescriptor &desc);
    void destroyBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle);
    void readBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle, const BufferReadDescriptor &desc);
}
