
#include "ProgramGL.hpp"
#include "ShaderGL.hpp"

namespace XE::Graphics::GL {
    ProgramGL::ProgramGL(const std::vector<std::tuple<ShaderType, std::string>> &sources) {
        for (const auto &sourceTuple : sources) {
            const ShaderType type = std::get<0>(sourceTuple);
            const std::string &code = std::get<1>(sourceTuple);

            m_shaders.emplace_back(new ShaderGL(type, code));
        }
    }

    ProgramGL::~ProgramGL() {
        m_shaders.clear();

        if (m_id) {
            ::glDeleteProgram(m_id);
        }
    }

    int ProgramGL::GetShaderCount() const {
        return int(m_shaders.size());
    }

    Shader* ProgramGL::GetShader(const int index) {
        return m_shaders[index].get();
    }

    int ProgramGL::GetUniformLoction(const std::string &name) const {
        return ::glGetUniformLocation(m_id, name.c_str());
    }

    int ProgramGL::GetAttributeLocation(const std::string &name) const {
        return ::glGetAttribLocation(m_id, name.c_str());
    }
}
