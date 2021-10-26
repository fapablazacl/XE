
#include <XE/Graphics/GL/BufferGL.h>

#include <XE/Graphics/GL/Conversion.h>
#include <cstdint>


namespace XE {
    BufferGL::BufferGL(const BufferDescriptor &desc) {
        const GLenum targetGL = convertToGL(desc.type);
        const GLenum usageGL = convertToGL(desc.usage, desc.access);

        glGenBuffers(1, &m_id);
        glBindBuffer(targetGL, m_id);
        glBufferData(targetGL, desc.size, desc.data, usageGL);
        glBindBuffer(targetGL, 0);

        m_size = desc.size;
        m_target = targetGL;
        m_usage = usageGL;
    }

    BufferGL::~BufferGL() {
        if (m_id) {
            glDeleteBuffers(1, &m_id);
        }
    }

    void BufferGL::read(std::byte* destination, const int size, const int offset, const int destinationOffset) const {
        const int finalSize = size ? size : m_size;

        glBindBuffer(m_target, m_id);
        glGetBufferSubData(m_target, offset, finalSize, &destination[destinationOffset]);
        glBindBuffer(m_target, 0);
    }

    void BufferGL::write(const std::byte *source, const int size, const int offset, const int sourceOffset) {
        const int finalSize = size ? size : m_size;

        glBindBuffer(m_target, m_id);
        glBufferSubData(m_target, offset, finalSize, &source[sourceOffset]);
        glBindBuffer(m_target, 0);
    }
}
