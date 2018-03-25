
#include "SubsetGL.hpp"
#include "BufferGL.hpp"

namespace XE::Graphics::GL {
    SubsetGL::SubsetGL(const SubsetDescriptor &desc) {
        ::glGenVertexArrays(1, &m_id);
        ::glBindVertexArray(m_id);

        ::glBindVertexArray(0);
    }

    SubsetGL::~SubsetGL() {
        if (m_id) {
            ::glDeleteVertexArrays(1, &m_id);
        }
    }
        
    int SubsetGL::GetBufferCount() const {
        return (int)m_buffers.size();
    }

    Buffer* SubsetGL::GetBuffer(const int index) {
        return m_buffers[index].get();
    }

    Buffer* SubsetGL::GetIndexBuffer() {
        return m_indexBuffer.get();
    }

    const Buffer* SubsetGL::GetBuffer(const int index) const {
        return m_buffers[index].get();
    }

    const Buffer* SubsetGL::GetIndexBuffer() const {
        return m_indexBuffer.get();
    }
}
