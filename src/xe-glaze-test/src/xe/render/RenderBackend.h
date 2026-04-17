
#pragma once 

#include <cstdint>

#include <xe/render/types.h>

namespace xe {
    /**
     * @brief render device dispatch table 
     * Allows to abstract away the underlying graphics API without resorting to polymorphism
     */
    struct RenderDeviceBackendVTable {
        Handle (*createBuffer)(RenderDeviceBackendContext *, const BufferDescriptor &) = nullptr;
        void (*destroyBuffer)(RenderDeviceBackendContext *, Handle) = nullptr;

        Handle (*createShaderProgram)(RenderDeviceBackendContext *, const ShaderProgramDescriptor &) = nullptr;
        void (*destroyShaderProgram)(RenderDeviceBackendContext *, Handle) = nullptr;

        Handle (*createVertexLayout)(RenderDeviceBackendContext *, const VertexLayoutDescriptor &) = nullptr;
        void (*destroyVertexLayout)(RenderDeviceBackendContext *, Handle) = nullptr;

        Handle (*createPipeline)(RenderDeviceBackendContext *, const PipelineDescriptor&) = nullptr;
        void (*destroyPipeline)(RenderDeviceBackendContext *, Handle) = nullptr;

        Handle (*createTexture)(RenderDeviceBackendContext *, const TextureDescriptor) = nullptr;
        void (*destroyTexture)(RenderDeviceBackendContext *, Handle) = nullptr;

        Handle (*createGeometry)(RenderDeviceBackendContext *, const GeometryDescriptor) = nullptr;
        void (*destroyGeometry)(RenderDeviceBackendContext *, Handle) = nullptr;

        void (*beginFrame)(RenderDeviceBackendContext *) = nullptr;
        void (*endFrame)(RenderDeviceBackendContext *) = nullptr;
        void (*present)(RenderDeviceBackendContext *) = nullptr;
    };
}
