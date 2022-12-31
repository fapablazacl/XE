
#ifndef __XE_GRAPHICS_ES2_BUFFER_ES2_HPP__
#define __XE_GRAPHICS_ES2_BUFFER_ES2_HPP__

#include <xe/Buffer.h>
#include <xe/graphics/BufferDescriptor.h>
#include <glad/glad.h>

namespace XE {
    enum class BufferUsage;
    enum class BufferAccess;
    enum class BufferType;

    class BufferES2 : public Buffer {
    public:
        BufferES2(const BufferDescriptor &desc);

        virtual ~BufferES2();

        std::size_t getSize() const override {
            return m_size;
        }

        void read(void* destination, const size_t size, const size_t offset, const size_t destinationOffset) const override;

        void write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset) override;

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
        std::size_t m_size;
    };
}

#endif
