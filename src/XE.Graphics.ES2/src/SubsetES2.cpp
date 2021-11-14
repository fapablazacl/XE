
#include <XE/Graphics/ES2/SubsetES2.h>
#include <XE/Graphics/ES2/BufferES2.h>
#include <XE/Graphics/ES2/ConversionES.h>

namespace XE {
    SubsetES::SubsetES(const SubsetDescriptor& desc) : descriptor(desc) {
        for (size_t i = 0; i < desc.buffers.size(); i++) {
            auto bufferGL = static_cast<const BufferES2*>(desc.buffers[i]);
            buffers.emplace_back(bufferGL);
        }

        indexBuffer = static_cast<const BufferES2*>(desc.indexBuffer);
    }


    void SubsetES::bind() const {
        for (size_t i = 0; i < descriptor.attribs.size(); i++) {
            const SubsetVertexAttrib& attrib = descriptor.attribs[i];
            const BufferES2* buffer = buffers[attrib.bufferIndex];

            glBindBuffer(buffer->getTarget(), buffer->getID());
            glEnableVertexAttribArray(attrib.shaderLocation);

            glVertexAttribPointer(
                attrib.shaderLocation,
                attrib.size,
                convertToES(attrib.type),
                convertToES(attrib.normalized),
                static_cast<GLsizei>(attrib.stride),
                reinterpret_cast<const void*>(attrib.bufferOffset)
            );
        }

        if (indexBuffer) {
            glBindBuffer(indexBuffer->getTarget(), indexBuffer->getID());
        }
    }


    void SubsetES::unbind() const {
        for (size_t i = 0; i < descriptor.attribs.size(); i++) {
            const SubsetVertexAttrib& attrib = descriptor.attribs[i];
            const BufferES2* buffer = buffers[attrib.bufferIndex];

            glBindBuffer(buffer->getTarget(), 0);
            glDisableVertexAttribArray(attrib.shaderLocation);
        }

        if (indexBuffer) {
            glBindBuffer(indexBuffer->getTarget(), 0);
        }
    }


    SubsetES::~SubsetES() {}
    
    int SubsetES::getBufferCount() const {
        return (int)buffers.size();
    }

    BufferES2* SubsetES::getBuffer(const int index) {
        return const_cast<BufferES2*>(buffers[index]);
    }

    BufferES2* SubsetES::getIndexBuffer() {
        return const_cast<BufferES2*>(indexBuffer);
    }

    const BufferES2* SubsetES::getBuffer(const int index) const {
        return buffers[index];
    }

    const BufferES2* SubsetES::getIndexBuffer() const {
        return indexBuffer;
    }
}
