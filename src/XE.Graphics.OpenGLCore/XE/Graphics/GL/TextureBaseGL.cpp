
#include "TextureBaseGL.hpp"

namespace XE::Graphics::GL {
    TextureBaseGL::TextureBaseGL(const GLenum target) {
        ::glGenTextures(1, &m_id);

        m_target = target;
    }

    TextureBaseGL::~TextureBaseGL() {
        if (m_id) {
            ::glDeleteTextures(1, &m_id);
        }
    }
}
