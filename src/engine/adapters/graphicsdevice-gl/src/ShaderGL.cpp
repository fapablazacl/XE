
#include <xe/graphics/gl/ShaderGL.h>

#include <cassert>
#include <stdexcept>

namespace XE {
    static GLenum s_shaderTypesGL[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};

    ShaderGL::ShaderGL(const ShaderType type, const std::string &source) {
        assert(source != "");

        m_type = type;
        m_source = source;
        m_typeGL = s_shaderTypesGL[int(type)];

        m_id = glCreateShader(m_typeGL);

        auto glsl = static_cast<const GLchar *const>(source.c_str());
        auto size = static_cast<GLsizei>(source.size());
        glShaderSource(m_id, 1, &glsl, &size);
        glCompileShader(m_id);

        // check for errors
        GLint status;
        glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            std::string msg = "XE::ShaderGL::ShaderGL:\n";
            {
                const GLint logsize = 4096;
                char buffer[logsize] = {};

                glGetShaderInfoLog(m_id, logsize, nullptr, buffer);

                msg += buffer;
            }

            msg += "\n\n" + source;

            throw std::runtime_error(msg);
        }
    }

    ShaderGL::~ShaderGL() {
        if (m_id) {
            glDeleteShader(m_id);
        }
    }

    ShaderType ShaderGL::getType() const { return m_type; }

    std::string ShaderGL::getSource() const { return m_source; }
} // namespace XE
