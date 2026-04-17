
#include "BackendTableGL.h"

#include <glaze/gl.hpp>
#include <xe/math/Vector.h>

namespace xe {

	//! Encapsulate immutable render state
	struct PipelineGL {
		gl::Program shaderProgram;

		vec4 clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		float clearDepth = 1.0f;

		gl::Flags<gl::ClearBufferMask> clearFlags;
	};

	struct BackendContextGL {
		std::vector<gl::BufferId> buffers;
		std::vector<PipelineGL> pipelines;
	};


	Handle BackendTableGL::createBuffer(BackendContext *ctx, const BufferDescriptor& desc)
	{
		gl::BufferId buffer = gl::createBuffers();
		gl::bindBuffer(gl::BufferTarget::eArrayBuffer, buffer);
		gl::bufferData(gl::BufferTarget::eArrayBuffer, desc.size, desc.data, gl::BufferUsage::eDynamicDraw);
		gl::bindBuffer(gl::BufferTarget::eArrayBuffer, {});

		return static_cast<Handle>(buffer.id);
	}

	void BackendTableGL::destroyBuffer(BackendContext *ctx, Handle buffer)
	{
		// NOTE: maybe we should check if the buffer is being used?
		gl::deleteBuffers(gl::BufferId{buffer});
	}

	void BackendTableGL::beginFrame(BackendContext *ctx)
	{
		gl::clearColor(0.2f, 0.2f, 8.0f, 1.0f);
		gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);
	}

	void BackendTableGL::endFrame(BackendContext *ctx)
	{
		gl::flush();
	}

	void BackendTableGL::present(BackendContext *ctx)
	{
		// no-op
	}
}
