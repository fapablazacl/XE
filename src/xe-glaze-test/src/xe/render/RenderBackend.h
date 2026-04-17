
#pragma once 

#include <cstdint>

#include <xe/render/types.h>

namespace xe {
    struct BackendContext {

    };

    struct BufferDescriptor {
        size_t size = 0;
        const void* data = nullptr;
    };
    
    //! eliminates polymorphic calls
    struct BackendTable {
        Handle (*createBuffer)(BackendContext *, const BufferDescriptor &) = nullptr;
        void (*destroyBuffer)(BackendContext *, Handle) = nullptr;

        void (*beginFrame)(BackendContext *) = nullptr;
        void (*endFrame)(BackendContext *) = nullptr;
        void (*present)(BackendContext *) = nullptr;
    };
}
