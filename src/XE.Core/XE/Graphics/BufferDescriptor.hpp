
#ifndef __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__
#define __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__

#include <cstddef>

namespace XE::Graphics {
    enum class BufferAccess {
        Static, Dynamic, Stream
    };

    enum class BufferUsage {
        Write, Read, Copy
    };

    enum class BufferType {
        Vertex,
        Index
    };

    struct BufferDescriptor {
        BufferType Type;
        BufferUsage Usage;
        BufferAccess Access;
        int Size;
        const std::byte *Data;
    };
}

#endif
