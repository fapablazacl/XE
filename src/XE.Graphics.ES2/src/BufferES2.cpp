
#include <XE/Graphics/ES2/BufferES2.h>
#include <XE/Graphics/ES2/Conversion.h>

#include <cstdint>

namespace XE {
    BufferES2::BufferES2(const BufferDescriptor &desc) {
        const GLenum targetGL = convertToGL(desc.type);
        const GLenum usageGL = convertToGL(desc.usage, desc.access);

        ::glGenBuffers(1, &m_id);
        ::glBindBuffer(targetGL, m_id);
        ::glBufferData(targetGL, desc.size, desc.data, usageGL);
        ::glBindBuffer(targetGL, 0);

        m_size = desc.size;
        m_target = targetGL;
        m_usage = usageGL;
    }

    BufferES2::~BufferES2() {
        if (m_id) {
            ::glDeleteBuffers(1, &m_id);
        }
    }

    void BufferES2::read(std::byte* destination, const int size, const int offset, const int destinationOffset) const {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        // ::glGetBufferSubData(m_target, offset, finalSize, &destination[destinationOffset]);
        // return false;
        ::glBindBuffer(m_target, 0);
    }

    void BufferES2::write(const std::byte *source, const int size, const int offset, const int sourceOffset) {
        const int finalSize = size ? size : m_size;

        ::glBindBuffer(m_target, m_id);
        ::glBufferSubData(m_target, offset, finalSize, &source[sourceOffset]);
        ::glBindBuffer(m_target, 0);
    }
}
