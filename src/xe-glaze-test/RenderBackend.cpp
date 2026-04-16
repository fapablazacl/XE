
#include "RenderBackend.h"

#include <glaze/gl.hpp>

namespace xe {

	struct BackendTableGL {
		static Buffer createBuffer(const BufferDescriptor&) {
			return 0;
		}

		static void destroyBuffer(Buffer buffer) {
			
		}

		static void beginFrame() {

		}

		static void endFrame() {

		}

		static void present() {

		}
	};
}
