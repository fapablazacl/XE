
#ifndef __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__
#define __XE_GRAPHICS_BUFFERDESCRIPTOR_HPP__

#include <XE/Predef.hpp>
#include <cstddef>

namespace XE {
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
        BufferType type;
        BufferUsage usage;
        BufferAccess access;
        int size;
        const std::byte *data;
    };
}

#endif
