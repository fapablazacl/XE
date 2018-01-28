
#ifndef __XE_GRAPHICS_GL_BUFFERGL_HPP__
#define __XE_GRAPHICS_GL_BUFFERGL_HPP__

#include <XE/Buffer.hpp>
#include <glad/glad.h>

namespace XE::Graphics::GL {
    class BufferGL : public Buffer {
    public:
        BufferGL(const GLenum target, const GLenum usage, const int size, const void *data);

        virtual ~BufferGL();

        virtual int GetSize() const override {
            return m_size;
        }

        virtual void Read(std::byte* destination, const int size, const int offset, const int destinationOffset) const override;

        virtual void Write(const std::byte *source, const int size, const int offset, const int sourceOffset) override;

    public:
        GLuint GetID() const {
            return m_id;
        }

        GLenum GetTarget() const {
            return m_target;
        }

        GLenum GetUsage() const {
            return m_usage;
        }

    private:
        GLuint m_id;
        GLenum m_target;
        GLenum m_usage;
        int m_size;
    };
}

#endif
