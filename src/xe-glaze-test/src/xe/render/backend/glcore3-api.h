
#pragma once 

#include <xe/render/types.h>

namespace xe {
	struct BackendContext;
	struct BufferDescriptor;
	
	Handle createBuffer(BackendContext *ctx, const BufferDescriptor&);

	void destroyBuffer(BackendContext *ctx, Handle buffer);

	void beginFrame(BackendContext *ctx);

	void endFrame(BackendContext *ctx);

	void present(BackendContext *ctx);
}
