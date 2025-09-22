
#ifndef __XE_GRAPHICS_GL_BUFFERGL_HPP__
#define XE_GRAPHICS_GL_BUFFERGL_HPP_

#include "gl.h"
#include "xe/Buffer.h"

#include <cstddef>

namespace XE {
    /**
     * A Wrapper for a Buffer
     */
    class BufferGL : public Buffer {
    public:
        BufferGL(GLenum target, GLenum usage, const void *data, GLsizei size);

        explicit BufferGL(GLuint bufferId);

        ~BufferGL() override;

        void read(void *destination, size_t size, size_t offset, size_t destinationOffset) const override;

        void write(const void *source, size_t size, size_t offset, size_t sourceOffset) override;

        explicit operator bool() const {
            return m_id != 0;
        }

        [[nodiscard]]
        explicit operator GLuint() const {
            return m_id;
        }

        GLuint getID() const {
            return m_id;
        }

        GLenum getTarget() const {
            return m_target;
        }

        std::size_t getSize() const override {
            return m_size;
        }

    private:
        GLuint m_id = 0;
        GLenum m_target;
        GLsizei m_size;
    };

} // namespace XE

namespace xe::gl {
    template <GLenum Target> class Buffer {
    public:
        Buffer() = default;

        Buffer(const void *data, GLsizei size, GLenum usage) {
            glGenBuffers(1, &m_id);
            glBindBuffer(Target, m_id);
            glBufferData(Target, size, data, usage);
            glBindBuffer(Target, 0);
        }

        GLuint getID() const {
            return m_id;
        }

        explicit operator bool() const {
            return m_id != 0;
        }

        [[nodiscard]]
        explicit operator GLuint() const {
            return m_id;
        }

    private:
        GLuint m_id = 0;
    };

    template <GLenum Target> void bind(const Buffer<Target> &buffer) {
        glBindBuffer(Target, buffer.getID());
    }
} // namespace xe::gl

#endif
