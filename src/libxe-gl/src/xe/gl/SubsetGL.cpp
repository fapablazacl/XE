
#include "SubsetGL.h"
#include "BufferGL.h"
#include "Conversion.h"
#include "xe/gl/gl.h"
#include "xe/graphics/Subset.h"

#include <cassert>
#include <cstddef>

namespace XE {
    SubsetGL::SubsetGL(const SubsetDescriptor &desc) : descriptor(desc) {
        for (std::size_t i = 0; i < desc.buffers.size(); i++) {
            const auto *bufferGL = dynamic_cast<const BufferGL *>(desc.buffers[i]);
            assert(bufferGL);
            buffers.emplace_back(bufferGL);
        }

        // setup the subset arrays based on the vertex format and the mapping information
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);

        for (std::size_t i = 0; i < desc.attribs.size(); i++) {
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
                reinterpret_cast<const void *>(attrib.bufferOffset)
            );
        }

        indexBuffer = dynamic_cast<const BufferGL *>(desc.indexBuffer);

        if (indexBuffer != nullptr) {
            glBindBuffer(indexBuffer->getTarget(), indexBuffer->getID());
        }

        glBindVertexArray(0);
    }

    SubsetGL::~SubsetGL() {
        if (id != 0u) {
            glDeleteVertexArrays(1, &id);
        }
    }

    int SubsetGL::getBufferCount() const {
        return (int)buffers.size();
    }

    BufferGL *SubsetGL::getBuffer(const int index) {
        return const_cast<BufferGL *>(buffers[index]);
    }

    BufferGL *SubsetGL::getIndexBuffer() {
        return const_cast<BufferGL *>(indexBuffer);
    }

    const BufferGL *SubsetGL::getBuffer(const int index) const {
        return buffers[index];
    }

    const BufferGL *SubsetGL::getIndexBuffer() const {
        return indexBuffer;
    }
} // namespace XE
