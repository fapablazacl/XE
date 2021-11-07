
#ifndef __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__
#define __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__

#include <XE/Predef.h>
#include <cstddef>

namespace XE {
    enum class BufferAccess {
        Static, Dynamic, Stream
    };

    enum class BufferUsage {
        Copy, Read, Write
    };

    enum class BufferType {
        Vertex,
        Index
    };

    struct BufferDescriptor {
        BufferType type;
        BufferUsage usage;
        BufferAccess access;
        int size;
        const std::byte *data;
    };
}

#endif
