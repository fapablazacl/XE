
#include <xe/graphics/gl/BufferGL.h>

#include <xe/graphics/gl/Conversion.h>
#include <cstdint>
#include <cassert>

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

    void BufferGL::read(void* destination, const size_t size, const size_t offset, const size_t destinationOffset) const {
        assert(destination);
        
        const int finalSize = size ? size : m_size;
        const auto ptr = reinterpret_cast<std::byte*>(destination);

        glBindBuffer(m_target, m_id);
        glGetBufferSubData(m_target, offset, finalSize, &ptr[destinationOffset]);
        glBindBuffer(m_target, 0);
    }
    

    void BufferGL::write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset) {
        assert(source);
        
        const int finalSize = size ? size : m_size;
        const auto ptr = reinterpret_cast<const std::byte*>(source);

        glBindBuffer(m_target, m_id);
        glBufferSubData(m_target, offset, finalSize, &ptr[sourceOffset]);
        glBindBuffer(m_target, 0);
    }
}
