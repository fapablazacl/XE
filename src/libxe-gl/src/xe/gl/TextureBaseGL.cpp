
#include "TextureBaseGL.h"
#include "xe/gl/gl.h"

namespace XE {
    TextureBaseGL::TextureBaseGL(const GLenum target) : m_target(target) {
        glGenTextures(1, &m_id);

        
    }

    TextureBaseGL::~TextureBaseGL() {
        if (m_id != 0u) {
            glDeleteTextures(1, &m_id);
        }
    }
} // namespace XE
