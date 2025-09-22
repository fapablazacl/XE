
#ifndef __XE_GRAPHICS_GL_TEXTUREBASEGL_HPP__
#define XE_GRAPHICS_GL_TEXTUREBASEGL_HPP_

#include "gl.h"

namespace XE {
    /**
     * @brief Structural base class for textures
     */
    class TextureBaseGL {
    public:
        explicit TextureBaseGL(GLenum target);

        virtual ~TextureBaseGL() = 0;

    
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
} // namespace XE

#endif
