
#include <XE/Graphics/ES2/SubsetES2.h>
#include <XE/Graphics/ES2/BufferES2.h>
#include <XE/Graphics/ES2/Conversion.h>

namespace XE {
    SubsetES::SubsetES(const SubsetDescriptor2& desc) {
        m_descriptor = desc;
    }

    SubsetES::~SubsetES() {
        if (m_id) {
            // glDeleteVertexArrays(1, &m_id);
        }
    }
        
    int SubsetES::getBufferCount() const {
        return (int)m_buffers.size();
    }

    BufferES2* SubsetES::getBuffer(const int index) {
        return m_buffers[index].get();
    }

    BufferES2* SubsetES::getIndexBuffer() {
        return m_indexBuffer.get();
    }

    const BufferES2* SubsetES::getBuffer(const int index) const {
        return m_buffers[index].get();
    }

    const BufferES2* SubsetES::getIndexBuffer() const {
        return m_indexBuffer.get();
    }
}
