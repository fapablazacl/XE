
#ifndef __XE_GRAPHICS_GL_BUFFERGL_HPP__
#define __XE_GRAPHICS_GL_BUFFERGL_HPP__

#include "glcore.h"
#include "xe/Buffer.h"

#include <cstddef>

namespace XE {
    /**
     * A Wrapper for a Buffer
     */
    class BufferGL : public Buffer {
    public:
        BufferGL(GLenum target, GLenum usage, const void * data, GLsizei size);

        explicit BufferGL(GLuint bufferId);

        virtual ~BufferGL();

        void read(void *destination, const size_t size, const size_t offset, const size_t destinationOffset) const;

        void write(const void *source, const size_t size, const size_t offset, const size_t sourceOffset);

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

        std::size_t getSize() const {
            return m_size;
        }

    private:
        GLuint m_id = 0;
        GLenum m_target;
        GLsizei m_size;
    };
} // namespace xe

#endif
