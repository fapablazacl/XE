
#include "BufferGL.hpp"

namespace XE::Graphics::GL {
    BufferGL::BufferGL(const GLenum target, const GLenum usage, const int size, const void *data) {
        ::glGenBuffers(1, &m_id);
        ::glBindBuffer(target, m_id);
        ::glBufferData(target, size, data, usage);
        ::glBindBuffer(target, 0);

        m_size = size;
        m_target = target;
        m_usage = usage;
    }

    BufferGL::~BufferGL() {
        if (this->id) {
            ::glDeleteBuffers(1, &m_id);
        }
    }

    void BufferGL::Read(void* destination, const int size, const int offset, const int destinationOffset) const {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        ::glGetBufferSubData(target, offset, finalSize, &destination[destinationOffset]);
        ::glBindBuffer(m_target, 0);
    }

    void BufferGL::Write(const void *source, const int size, const int offset, const int sourceOffset) {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        ::glBufferSubData(m_target, offset, finalSize, &source[sourceOffset]);
        ::glBindBuffer(m_target, 0);
    }
}
