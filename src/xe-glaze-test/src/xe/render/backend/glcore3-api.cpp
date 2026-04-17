

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <vector>

#include "glcore3-api.h"

namespace xe {
	struct PipelineGL {
		xe::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;
		glaze::Unique<gl::Program> shaderProgram;
	};

	struct RenderDeviceBackendContextGL : RenderDeviceBackendContext {
		std::vector<glaze::Unique<gl::BufferId>> buffers;

		Handle createBuffer(const BufferDescriptor& desc) {
			auto const target = gl::BufferTarget::eArrayBuffer;
			auto const usage = gl::BufferUsage::eDynamicDraw;

			auto buffer = glaze::makeUnique<gl::BufferId>();
			gl::bindBuffer(target, buffer);
			gl::bufferData(target, desc.size, desc.data, usage);
			gl::bindBuffer(target, {});

			Handle const bufferHandle = buffers.size();

			// TODO: Search for a free Handle
			buffers.push_back(std::move(buffer));

			return bufferHandle;
		}

		void destroyBuffer(Handle bufferHandle) {
			buffers[bufferHandle].reset({});
		}

		void beginFrame() {
			gl::clearColor(0.2f, 0.2f, 8.0f, 1.0f);
			gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);
		}

		void endFrame() {
			gl::flush();
		}

		void present() {
			// no-op
		}
	};

	Handle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc) {
		return static_cast<RenderDeviceBackendContextGL*>(ctx)->createBuffer(desc);
	}

	void destroyBufferGL(RenderDeviceBackendContext* ctx, Handle buffer) {
		static_cast<RenderDeviceBackendContextGL*>(ctx)->destroyBuffer(buffer);
	}

	void beginFrameGL(RenderDeviceBackendContext* ctx) {
		static_cast<RenderDeviceBackendContextGL*>(ctx)->beginFrame();
	}

	void endFrameGL(RenderDeviceBackendContext* ctx) {
		static_cast<RenderDeviceBackendContextGL*>(ctx)->endFrame();
	}

	void presentGL(RenderDeviceBackendContext* ctx) {
		static_cast<RenderDeviceBackendContextGL*>(ctx)->present();
	}

	void initializeBackendTableGL(RenderDeviceBackendVTable* vtable) {
		vtable->createBuffer = &createBufferGL;
		vtable->destroyBuffer = &destroyBufferGL;
		vtable->beginFrame = &beginFrameGL;
		vtable->endFrame = &endFrameGL;
		vtable->present = &presentGL;
	}
}
