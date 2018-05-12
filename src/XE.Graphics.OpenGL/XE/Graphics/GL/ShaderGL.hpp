
#ifndef __XE_GRAPHICS_GL_SHADERGL_HPP__
#define __XE_GRAPHICS_GL_SHADERGL_HPP__

#include <XE/Graphics/Shader.hpp>
#include <glad/glad.h>

namespace XE::Graphics::GL {
    class ShaderGL : public Shader {
    public:
        ShaderGL(const ShaderType type, const std::string &source);

        virtual ~ShaderGL();

        virtual ShaderType GetType() const override;

        virtual std::string GetSource() const override;

        GLuint GetID() const {
            return m_id;
        }

    private:
        ShaderType m_type;
        std::string m_source;

        GLenum m_id;
        GLenum m_typeGL;
    };
}

#endif
