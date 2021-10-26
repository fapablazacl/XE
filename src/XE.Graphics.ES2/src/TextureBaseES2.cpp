
#include <XE/Graphics/ES2/TextureBaseES2.h>

namespace XE {
    TextureBaseGL::TextureBaseGL(const GLenum target) {
        glGenTextures(1, &m_id);

        m_target = target;
    }

    TextureBaseGL::~TextureBaseGL() {
        if (m_id) {
            glDeleteTextures(1, &m_id);
        }
    }
}
