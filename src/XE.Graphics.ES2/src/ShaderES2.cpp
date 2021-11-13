
#include <XE/Graphics/ES2/ShaderES2.h>

#include <stdexcept>

namespace XE {
    static GLenum s_shaderTypesGL[] = {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER
    };

    ShaderES::ShaderES(const ShaderType type, const std::string &source) {
        m_type = type;
        m_source = source;
        m_typeGL = s_shaderTypesGL[int(type)];

        m_id = glCreateShader(m_typeGL);

        auto glsl = static_cast<const GLchar *const>(source.c_str());
        auto size = static_cast<GLsizei>(source.size());
        glShaderSource (m_id, 1, &glsl, &size);
        glCompileShader(m_id);

        // check for errors
        GLint status;
        glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
            const GLint logsize = 4096;
            char buffer[logsize] = {};
                
            glGetShaderInfoLog(m_id, logsize, nullptr, buffer);

            throw std::runtime_error(buffer);
        }
    }

    ShaderES::~ShaderES() {
        if (m_id) {
            glDeleteShader(m_id);
        }
    }

    ShaderType ShaderES::getType() const {
        return m_type;
    }

    std::string ShaderES::getSource() const {
        return m_source;
    }
}
