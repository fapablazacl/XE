
#ifndef __XE_GRAPHICS_GL_TEXTUREBASEGL_HPP__
#define __XE_GRAPHICS_GL_TEXTUREBASEGL_HPP__

#include "glcore.h"

namespace XE {
    /**
     * @brief Structural base class for textures
     */
    class TextureBaseGL {
    public:
        explicit TextureBaseGL(const GLenum target);

        virtual ~TextureBaseGL() = 0;

    public:
        GLuint GetID() const {
            return m_id;
        }

        GLenum GetTarget() const {
            return m_target;
        }

    protected:
        GLuint m_id = 0;
        GLenum m_target;
    };
}

#endif
