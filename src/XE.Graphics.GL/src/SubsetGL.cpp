
#include <XE/Graphics/GL/SubsetGL.h>
#include <XE/Graphics/GL/BufferGL.h>
#include <XE/Graphics/GL/Conversion.h>


namespace XE {
    SubsetGL::SubsetGL(const SubsetDescriptor &desc) : descriptor(desc) {
        for (std::size_t i=0; i<desc.buffers.size(); i++) {
            auto bufferGL = static_cast<const BufferGL*>(desc.buffers[i]);
            assert(bufferGL);
            buffers.emplace_back(bufferGL);
        }

        // setup the subset arrays based on the vertex format and the mapping information
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);

        for (std::size_t i=0; i<desc.attribs.size(); i++) {
            const SubsetVertexAttrib &attrib = desc.attribs[i];
            const BufferGL *buffer = buffers[attrib.bufferIndex];

            glBindBuffer(buffer->getTarget(), buffer->getID());
            glEnableVertexAttribArray(attrib.shaderLocation);

            glVertexAttribPointer(
                attrib.shaderLocation, 
                attrib.size, 
                convertToGL(attrib.type), 
                convertToGL(attrib.normalized), 
                static_cast<GLsizei>(attrib.stride),
                reinterpret_cast<const void*>(attrib.bufferOffset)
            );
        }
        
        indexBuffer = static_cast<const BufferGL*>(desc.indexBuffer);

        if (indexBuffer) {
            glBindBuffer(indexBuffer->getTarget(), indexBuffer->getID());
        }

        glBindVertexArray(0);
    }


    SubsetGL::~SubsetGL() {
        if (id) {
            glDeleteVertexArrays(1, &id);
        }
    }
        
    int SubsetGL::getBufferCount() const {
        return (int)buffers.size();
    }

    BufferGL* SubsetGL::getBuffer(const int index) {
        return const_cast<BufferGL*>(buffers[index]);
    }

    BufferGL* SubsetGL::getIndexBuffer() {
        return const_cast<BufferGL*>(indexBuffer);
    }

    const BufferGL* SubsetGL::getBuffer(const int index) const {
        return buffers[index];
    }

    const BufferGL* SubsetGL::getIndexBuffer() const {
        return indexBuffer;
    }
}
