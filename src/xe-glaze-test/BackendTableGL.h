
#pragma once 

#include "RenderBackend.h"

namespace xe {
	struct BackendTableGL {
		static Buffer createBuffer(const BufferDescriptor&);

		static void destroyBuffer(Buffer buffer);

		static void beginFrame();

		static void endFrame();

		static void present();
	};
}
