
#ifndef __XE_GRAPHICS_GL_SHADERGL_HPP__
#define __XE_GRAPHICS_GL_SHADERGL_HPP__

#include <xe/graphics/Shader.h>

#include "glcore.h"

namespace XE {
    class ShaderGL : public Shader {
    public:
        ShaderGL(const ShaderType type, const std::string &source);

        virtual ~ShaderGL();

        virtual ShaderType getType() const override;

        virtual std::string getSource() const override;

        GLuint GetID() const { return m_id; }

    private:
        ShaderType m_type;
        std::string m_source;

        GLuint m_id;
        GLenum m_typeGL;
    };
} // namespace XE

#endif
