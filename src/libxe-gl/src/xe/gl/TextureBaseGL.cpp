
#include "TextureBaseGL.h"

namespace xe {
    TextureBaseGL::TextureBaseGL(const GLenum target) : m_target(target) {
        glGenTextures(1, &m_id);
    }

    TextureBaseGL::~TextureBaseGL() {
        if (m_id) {
            glDeleteTextures(1, &m_id);
        }
    }
} // namespace xe
