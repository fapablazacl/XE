
#include <xe/graphics/gl/TextureBaseGL.h>

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
} // namespace XE
