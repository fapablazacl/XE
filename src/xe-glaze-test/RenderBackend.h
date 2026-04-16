
#pragma once 

#include <cstdint>

namespace xe {
    typedef struct BackendContext;

    using Buffer = uint32_t;

    struct BufferDescriptor {
        size_t size;
        const void* data = nullptr;
    };
    
    // eliminates polymorphic calls
    struct BackendTable {
        Buffer (*createBuffer)(const BufferDescriptor &) = nullptr;
        void (*destroyBuffer)(Buffer) = nullptr;

        void (*beginFrame)() = nullptr;
        void (*endFrame)() = nullptr;
        void (*present)() = nullptr;
    };


	template<class BackendT>
	BackendTable createBackendTableGL() {
		BackendTable vtable;

		vtable.createBuffer = &BackendT::createBuffer;
		vtable.destroyBuffer = &BackendT::destroyBuffer;
		vtable.beginFrame = &BackendT::beginFrame;
		vtable.endFrame = &BackendT::endFrame;
		vtable.present = &BackendT::present;

		return vtable;
	}
}
