
#include <xe/graphics/ES2/BufferES2.h>
#include <xe/graphics/ES2/ConversionES.h>

#include <cstdint>

namespace XE {
    BufferES2::BufferES2(const BufferDescriptor &desc) {
        const GLenum targetGL = convertToES(desc.type);
        const GLenum usageGL = convertToES(desc.usage, desc.access);

        glGenBuffers(1, &m_id);
        glBindBuffer(targetGL, m_id);
        glBufferData(targetGL, desc.size, desc.data, usageGL);
        glBindBuffer(targetGL, 0);

        m_size = desc.size;
        m_target = targetGL;
        m_usage = usageGL;
    }


    BufferES2::~BufferES2() {
        if (m_id) {
            glDeleteBuffers(1, &m_id);
        }
    }


    void BufferES2::read(void* destination, const size_t size, const size_t offset, const size_t destinationOffset) const {
        return;
    }

    
    void BufferES2::write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset) {
        const size_t finalSize = size ? size : m_size;
        const auto ptr = reinterpret_cast<const std::byte*>(source);

        glBindBuffer(m_target, m_id);
        glBufferSubData(m_target, offset, finalSize, &ptr[sourceOffset]);
        glBindBuffer(m_target, 0);
    }
}
