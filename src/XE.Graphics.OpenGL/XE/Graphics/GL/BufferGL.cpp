
#include "BufferGL.hpp"
#include "Conversion.hpp"
#include <cstdint>

namespace XE::Graphics::GL {
    BufferGL::BufferGL(const BufferDescriptor &desc) {
        const GLenum targetGL = ConvertToGL(desc.type);
        const GLenum usageGL = ConvertToGL(desc.usage, desc.access);

        ::glGenBuffers(1, &m_id);
        ::glBindBuffer(targetGL, m_id);
        ::glBufferData(targetGL, desc.size, desc.data, usageGL);
        ::glBindBuffer(targetGL, 0);

        m_size = desc.size;
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
