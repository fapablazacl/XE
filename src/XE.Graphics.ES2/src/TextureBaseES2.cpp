
#include <XE/Graphics/ES2/TextureBaseES2.h>

namespace XE {
    TextureBaseES::TextureBaseES(const GLenum target) {
        glGenTextures(1, &m_id);

        m_target = target;
    }

    TextureBaseES::~TextureBaseES() {
        if (m_id) {
            glDeleteTextures(1, &m_id);
        }
    }
}
