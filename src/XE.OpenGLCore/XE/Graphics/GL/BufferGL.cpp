
#include "BufferGL.hpp"
#include "Conversion.hpp"
#include <cstdint>

namespace XE::Graphics::GL {
    BufferGL::BufferGL(const BufferType type, const BufferUsage usage, const BufferAccess access, const int size, const void *data) {
        const GLenum targetGL = ConvertToGL(type);
        const GLenum usageGL = ConvertToGL(usage, access);

        ::glGenBuffers(1, &m_id);
        ::glBindBuffer(targetGL, m_id);
        ::glBufferData(targetGL, size, data, usageGL);
        ::glBindBuffer(targetGL, 0);

        m_size = size;
        m_target = targetGL;
        m_usage = usageGL;
    }

    BufferGL::~BufferGL() {
        if (m_id) {
            ::glDeleteBuffers(1, &m_id);
        }
    }

    void BufferGL::Read(std::byte* destination, const int size, const int offset, const int destinationOffset) const {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        ::glGetBufferSubData(m_target, offset, finalSize, &destination[destinationOffset]);
        ::glBindBuffer(m_target, 0);
    }

    void BufferGL::Write(const std::byte *source, const int size, const int offset, const int sourceOffset) {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        ::glBufferSubData(m_target, offset, finalSize, &source[sourceOffset]);
        ::glBindBuffer(m_target, 0);
    }
}
