
#ifndef __XE_GRAPHICS_GL_SHADERGL_HPP__
#define __XE_GRAPHICS_GL_SHADERGL_HPP__

#include <glad/glad.h>
#include <xe/graphics/Shader.h>

namespace XE {
    class ShaderES : public Shader {
    public:
        ShaderES(const ShaderType type, const std::string &source);

        virtual ~ShaderES();

        virtual ShaderType getType() const override;

        virtual std::string getSource() const override;

        GLuint GetID() const { return m_id; }

    private:
        ShaderType m_type;
        std::string m_source;

        GLenum m_id;
        GLenum m_typeGL;
    };
} // namespace XE

#endif
