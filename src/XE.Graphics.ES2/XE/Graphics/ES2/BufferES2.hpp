
#ifndef __XE_GRAPHICS_ES2_BUFFER_ES2_HPP__
#define __XE_GRAPHICS_ES2_BUFFER_ES2_HPP__

#include <XE/Buffer.hpp>
#include <XE/Graphics/BufferDescriptor.hpp>
#include <glad/es2/glad.h>

namespace XE {
    enum class BufferUsage;
    enum class BufferAccess;
    enum class BufferType;

    class BufferES2 : public Buffer {
    public:
        BufferES2(const BufferDescriptor &desc);

        virtual ~BufferES2();

        virtual int getSize() const override {
            return m_size;
        }

        virtual void read(std::byte* destination, const int size, const int offset, const int destinationOffset) const override;

        virtual void write(const std::byte *source, const int size, const int offset, const int sourceOffset) override;

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
