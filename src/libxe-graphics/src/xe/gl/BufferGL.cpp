
#include "BufferGL.h"

#include "Conversion.h"
#include <cassert>
#include <cstdint>

namespace xe {
    BufferGL::BufferGL(GLenum target, GLenum usage, const void *data, GLsizei size) : m_target(target), m_size(size) {
        glGenBuffers(1, &m_id);
        glBindBuffer(target, m_id);
        glBufferData(target, size, data, usage);
        glBindBuffer(target, 0);
    }

    BufferGL::~BufferGL() {
        if (m_id) {
            glDeleteBuffers(1, &m_id);
        }
    }

    void BufferGL::read(void *destination, const size_t size, const size_t offset, const size_t destinationOffset) const {
        assert(destination);

        const size_t finalSize = size ? size : m_size;
        const auto ptr = reinterpret_cast<std::byte *>(destination);

        glBindBuffer(m_target, m_id);
        glGetBufferSubData(m_target, offset, finalSize, &ptr[destinationOffset]);
        glBindBuffer(m_target, 0);
    }

    void BufferGL::write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset) {
        assert(source);

        const size_t finalSize = size ? size : m_size;
        const auto ptr = reinterpret_cast<const std::byte *>(source);

        glBindBuffer(m_target, m_id);
        glBufferSubData(m_target, offset, finalSize, &ptr[sourceOffset]);
        glBindBuffer(m_target, 0);
    }
} // namespace xe
