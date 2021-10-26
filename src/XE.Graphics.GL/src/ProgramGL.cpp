
#include <XE/Graphics/GL/ProgramGL.h>
#include <XE/Graphics/GL/ShaderGL.h>

#include <stdexcept>
#include <iostream>

namespace XE {
    ProgramGL::ProgramGL(const ProgramDescriptor &desc) {
        assert(desc.sources.size() > 0);
        
        for (const auto &source: desc.sources) {
            m_shaders.emplace_back(new ShaderGL(source.type, source.text));
        }

        m_id = glCreateProgram();

        for (auto &shader : m_shaders) {
            glAttachShader(m_id, shader->GetID());
        }

        glLinkProgram(m_id);

        GLint status;
        glGetProgramiv(m_id, GL_LINK_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            std::string msg = "ProgramGL::ProgramGL:\n";
            {
                const GLint logsize = 4096;
                char buffer[logsize] = {};
                    
                glGetProgramInfoLog(m_id, logsize, nullptr, buffer);
                
                msg += buffer;
            }
            
            std::cerr << msg << std::endl;
            
            throw std::runtime_error(msg);
        }
    }

    ProgramGL::~ProgramGL() {
        m_shaders.clear();

        if (m_id) {
            glDeleteProgram(m_id);
        }
    }

    int ProgramGL::getShaderCount() const {
        return int(m_shaders.size());
    }

    Shader* ProgramGL::getShader(const int index) {
        return m_shaders[index].get();
    }

    int ProgramGL::getUniformLoction(const std::string &name) const {
        return glGetUniformLocation(m_id, name.c_str());
    }

    int ProgramGL::getAttributeLocation(const std::string &name) const {
        return glGetAttribLocation(m_id, name.c_str());
    }
}
