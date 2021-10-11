
#include <XE/Graphics/GL/SubsetGL.h>
#include <XE/Graphics/GL/BufferGL.h>
#include <XE/Graphics/GL/Conversion.h>


namespace XE {
    SubsetGL::SubsetGL(const SubsetDescriptor2 &desc) {
        // take ownership for the buffers
        for (size_t i=0; i<desc.bufferCount; i++) {
            auto bufferGL = static_cast<const BufferGL*>(desc.buffers[i]);
            m_buffers.emplace_back(bufferGL);
        }

        // setup the subset arrays based on the vertex format and the mapping information
        ::glGenVertexArrays(1, &m_id);
        ::glBindVertexArray(m_id);

        for (size_t i=0; i<desc.attribCount; i++) {
            const SubsetVertexAttrib &attrib = desc.attribs[i];
            const BufferGL *buffer = m_buffers[attrib.bufferIndex];

            ::glBindBuffer(buffer->GetTarget(), buffer->GetID());
            ::glEnableVertexAttribArray(attrib.shaderLocation);

            ::glVertexAttribPointer(
                attrib.shaderLocation, 
                attrib.size, 
                convertToGL(attrib.type), 
                convertToGL(attrib.normalized), 
                attrib.stride,
                reinterpret_cast<const void*>(attrib.bufferOffset)
            );
        }
        
        m_indexBuffer = static_cast<const BufferGL*>(desc.indexBuffer);

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
        
    int SubsetGL::getBufferCount() const {
        return (int)m_buffers.size();
    }

    BufferGL* SubsetGL::getBuffer(const int index) {
        return const_cast<BufferGL*>(m_buffers[index]);
    }

    BufferGL* SubsetGL::getIndexBuffer() {
        return const_cast<BufferGL*>(m_indexBuffer);
    }

    const BufferGL* SubsetGL::getBuffer(const int index) const {
        return m_buffers[index];
    }

    const BufferGL* SubsetGL::getIndexBuffer() const {
        return m_indexBuffer;
    }
}
