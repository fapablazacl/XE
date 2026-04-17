
#pragma once 

#include "RenderBackend.h"

#include <glaze/raii.hpp>
#include <vector>

namespace xe {
	struct BackendTableGL {
		static Buffer createBuffer(BackendContext *ctx, const BufferDescriptor&);

		static void destroyBuffer(BackendContext *ctx, Buffer buffer);

		static void beginFrame(BackendContext *ctx);

		static void endFrame(BackendContext *ctx);

		static void present(BackendContext *ctx);
	};
}
