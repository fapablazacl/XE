

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <vector>

#include "glcore3-api.h"

namespace xe {
	struct BackendContextGLCore3 {
		std::vector<glaze::Unique<gl::BufferId>> buffers;
	};

	Handle createBuffer(BackendContext *ctx, const BufferDescriptor& desc) {
		gl::BufferId buffer = gl::createBuffers();
		gl::bindBuffer(gl::BufferTarget::eArrayBuffer, buffer);
		gl::bufferData(gl::BufferTarget::eArrayBuffer, desc.size, desc.data, gl::BufferUsage::eDynamicDraw);
		gl::bindBuffer(gl::BufferTarget::eArrayBuffer, {});

		return static_cast<Handle>(buffer.id);
	}

	void destroyBuffer(BackendContext *ctx, Handle buffer) {
		// NOTE: maybe we should check if the buffer is being used?
		gl::deleteBuffers(gl::BufferId{buffer});
	}

	void beginFrame(BackendContext *ctx) {
		gl::clearColor(0.2f, 0.2f, 8.0f, 1.0f);
		gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);
	}

	void endFrame(BackendContext *ctx) {
		gl::flush();
	}

	void present(BackendContext *ctx) {
		// no-op
	}
}
