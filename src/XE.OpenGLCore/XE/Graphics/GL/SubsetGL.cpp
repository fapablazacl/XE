
#include "SubsetGL.hpp"
#include "BufferGL.hpp"
#include "Conversion.hpp"

namespace XE::Graphics::GL {
    SubsetGL::SubsetGL(SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) {        
        // take ownership for the buffers
        for (auto &buffer : buffers) {
            auto bufferGL = dynamic_cast<BufferGL*>(buffer.get());
            m_buffers.emplace_back(bufferGL);
            buffer.release();
        }

        m_indexBuffer.reset((BufferGL*)indexBuffer.release());

        // setup the subset arrays based on the vertex format and the mapping information
        ::glGenVertexArrays(1, &m_id);
        ::glBindVertexArray(m_id);

        int attribIndex = 0;
        for (const VertexAttribute &attrib : desc.attributes) {
            auto mappingIt = bufferMapping.find(attrib.name);
            if (mappingIt == bufferMapping.end()) {
                continue;
            }
 
            const int bufferIndex = mappingIt->second;
            const BufferGL *buffer = m_buffers[bufferIndex].get();

            ::glBindBuffer(buffer->GetTarget(), buffer->GetID());
            ::glEnableVertexAttribArray(attribIndex);
            ::glVertexAttribPointer(attribIndex, attrib.size, ConvertToGL(attrib.type), GL_FALSE, 0, 0);

            attribIndex++;
        }

        if (m_indexBuffer) {
            ::glBindBuffer(m_indexBuffer->GetTarget(), m_indexBuffer->GetID());
        }
        
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

    BufferGL* SubsetGL::GetBuffer(const int index) {
        return m_buffers[index].get();
    }

    BufferGL* SubsetGL::GetIndexBuffer() {
        return m_indexBuffer.get();
    }

    const BufferGL* SubsetGL::GetBuffer(const int index) const {
        return m_buffers[index].get();
    }

    const BufferGL* SubsetGL::GetIndexBuffer() const {
        return m_indexBuffer.get();
    }
}
