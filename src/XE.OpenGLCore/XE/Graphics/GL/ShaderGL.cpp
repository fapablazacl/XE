
#include "ShaderGL.hpp"

namespace XE::Graphics::GL {
    static GLenum g_shaderTypesGL[] = {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER
    };

    ShaderGL::ShaderGL(const ShaderType type, const std::string &source) {
        m_type = type;
        m_source = source;
        m_typeGL = g_shaderTypesGL[type];

        m_id = ::glCreateShader(m_typeGL);

        auto source = static_cast<const GLchar *const>(glsl.c_str());
        auto size = static_cast<GLsizei>(glsl.size());
        glShaderSource (m_id, 1, &source, &size);
        glCompileShader(m_id);

        // check for errors
        GLint status;
        glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            const GLint logsize = 4096;
            char buffer[logsize] = {};
                
            glGetShaderInfoLog(m_id, logsize, nullptr, buffer);
                
            std::cerr << buffer << std::endl;

            throw std::runtime_error(buffer);
        }

    }

    ShaderGL::~ShaderGL() {

    }

    ShaderType ShaderGL::GetType() const {
        return m_type;
    }

    std::string ShaderGL::GetSource() const {
        return m_source;
    }
}
