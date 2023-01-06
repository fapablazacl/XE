
#ifndef __XE_GRAPHICS_GL_BUFFERGL_HPP__
#define __XE_GRAPHICS_GL_BUFFERGL_HPP__

#include <xe/Buffer.h>
#include <xe/graphics/BufferDescriptor.h>

#include "glcore.h"

namespace XE {
    enum class BufferUsage;
    enum class BufferAccess;
    enum class BufferType;

    class BufferGL : public Buffer {
    public:
        BufferGL(const BufferDescriptor &desc);

        virtual ~BufferGL();

        virtual size_t getSize() const override {
            return m_size;
        }

        virtual void read(void* destination, const size_t size, const size_t offset, const size_t destinationOffset) const override;

        virtual void write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset) override;

    public:
        GLuint getID() const {
            return m_id;
        }

        GLenum getTarget() const {
            return m_target;
        }

        GLenum getUsage() const {
            return m_usage;
        }

    private:
        GLuint m_id;
        GLenum m_target;
        GLenum m_usage;
        size_t m_size;
    };
}

#endif
