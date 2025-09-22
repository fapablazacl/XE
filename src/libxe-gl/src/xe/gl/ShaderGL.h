
#ifndef __XE_GRAPHICS_GL_SHADERGL_HPP__
#define XE_GRAPHICS_GL_SHADERGL_HPP_

#include <xe/graphics/Shader.h>

#include "gl.h"

namespace XE {
    class ShaderGL : public Shader {
    public:
        ShaderGL(ShaderType type, const std::string &source);

        ~ShaderGL() override;

        ShaderType getType() const override;

        std::string getSource() const override;

        GLuint GetID() const {
            return m_id;
        }

    private:
        ShaderType m_type;
        std::string m_source;

        GLuint m_id;
        GLenum m_typeGL;
    };
} // namespace XE

#endif
