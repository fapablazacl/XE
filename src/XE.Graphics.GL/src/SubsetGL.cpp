
#include <XE/Graphics/GL/SubsetGL.hpp>
#include <XE/Graphics/GL/BufferGL.hpp>
#include <XE/Graphics/GL/Conversion.hpp>


namespace XE {
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
            ::glVertexAttribPointer(attribIndex, attrib.size, convertToGL(attrib.type), GL_FALSE, 0, 0);

            attribIndex++;
        }

        if (m_indexBuffer) {
            ::glBindBuffer(m_indexBuffer->GetTarget(), m_indexBuffer->GetID());
        }
        
        ::glBindVertexArray(0);

        m_descriptor = desc;
    }

    SubsetGL::~SubsetGL() {
        if (m_id) {
            ::glDeleteVertexArrays(1, &m_id);
        }
    }
        
    int SubsetGL::getBufferCount() const {
        return (int)m_buffers.size();
    }

    BufferGL* SubsetGL::getBuffer(const int index) {
        return m_buffers[index].get();
    }

    BufferGL* SubsetGL::getIndexBuffer() {
        return m_indexBuffer.get();
    }

    const BufferGL* SubsetGL::getBuffer(const int index) const {
        return m_buffers[index].get();
    }

    const BufferGL* SubsetGL::getIndexBuffer() const {
        return m_indexBuffer.get();
    }
}
