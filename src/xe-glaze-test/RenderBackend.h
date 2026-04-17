
#pragma once 

#include <cstdint>

namespace xe {
    struct BackendContext;

    enum class HandleType {
        Buffer,
        Texture
    };

    using Handle = uint32_t;
    using Buffer = uint32_t;    // deprecated. use Handle instead

    struct BufferDescriptor {
        size_t size = 0;
        const void* data = nullptr;
    };
    
    //! eliminates polymorphic calls
    struct BackendTable {
        Buffer (*createBuffer)(BackendContext *, const BufferDescriptor &) = nullptr;
        void (*destroyBuffer)(BackendContext *, Buffer) = nullptr;

        void (*beginFrame)(BackendContext *) = nullptr;
        void (*endFrame)(BackendContext *) = nullptr;
        void (*present)(BackendContext *) = nullptr;
    };

	template<class BackendT>
	BackendTable createBackendTable() {
		BackendTable vtable;

		vtable.createBuffer = &BackendT::createBuffer;
		vtable.destroyBuffer = &BackendT::destroyBuffer;
		vtable.beginFrame = &BackendT::beginFrame;
		vtable.endFrame = &BackendT::endFrame;
		vtable.present = &BackendT::present;

		return vtable;
	}
}
