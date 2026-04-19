
#pragma once

#include <cstdint>

#include <xe/render/types.h>

namespace xe {
    /**
     * @brief render device dispatch table
     * Allows to abstract away the underlying graphics API without resorting to polymorphism.
     * All handle-typed fields use the phantom-typed aliases from xe::types, so cross-resource
     * mix-ups (e.g. destroying a TextureHandle via destroyBuffer) are compile-time errors.
     */
    struct RenderDeviceBackendVTable {
        RenderDeviceBackendContext* (*createContext)() = nullptr;
        void (*destroyContext)(RenderDeviceBackendContext*) = nullptr;

        BufferHandle (*createBuffer)(RenderDeviceBackendContext *, const BufferDescriptor &) = nullptr;
        void (*destroyBuffer)(RenderDeviceBackendContext *, BufferHandle) = nullptr;
        void (*readBuffer)(RenderDeviceBackendContext *, BufferHandle, const BufferReadDescriptor &) = nullptr;

        ShaderHandle (*createShaderProgram)(RenderDeviceBackendContext *, const ShaderProgramDescriptor &) = nullptr;
        void (*destroyShaderProgram)(RenderDeviceBackendContext *, ShaderHandle) = nullptr;

		TextureHandle (*createTexture)(RenderDeviceBackendContext*, const TextureDescriptor&) = nullptr;
		void (*destroyTexture)(RenderDeviceBackendContext*, TextureHandle) = nullptr;
		void (*updateTexture)(RenderDeviceBackendContext*, TextureHandle, const TextureUpdateDescriptor&) = nullptr;
		void (*readTexture)(RenderDeviceBackendContext*, TextureHandle, const TextureReadDescriptor&) = nullptr;

        VertexLayoutHandle (*createVertexLayout)(RenderDeviceBackendContext *, const VertexLayoutDescriptor &) = nullptr;
        void (*destroyVertexLayout)(RenderDeviceBackendContext *, VertexLayoutHandle) = nullptr;

        PipelineHandle (*createPipeline)(RenderDeviceBackendContext *, const PipelineDescriptor&) = nullptr;
        void (*destroyPipeline)(RenderDeviceBackendContext *, PipelineHandle) = nullptr;

        GeometryHandle (*createGeometry)(RenderDeviceBackendContext *, const GeometryDescriptor&) = nullptr;
        void (*destroyGeometry)(RenderDeviceBackendContext *, GeometryHandle) = nullptr;

        void (*beginFrame)(RenderDeviceBackendContext *) = nullptr;
        void (*endFrame)(RenderDeviceBackendContext *) = nullptr;
        void (*present)(RenderDeviceBackendContext *) = nullptr;
    };
}
