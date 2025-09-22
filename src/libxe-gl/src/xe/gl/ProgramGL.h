
#ifndef __XE_GRAPHICS_GL_PROGRAMGL_HPP__
#define XE_GRAPHICS_GL_PROGRAMGL_HPP_

#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <xe/graphics/Program.h>

#include "gl.h"

namespace XE {
    enum class ShaderType;
    struct ProgramDescriptor;
} // namespace XE

namespace XE {
    class ShaderGL;
    class ProgramGL : public Program {
    public:
        ProgramGL(const ProgramDescriptor &desc);

        ~ProgramGL() override;

        int getShaderCount() const override;

        Shader *getShader(int index) override;

        GLuint GetID() const {
            return m_id;
        }

    
        int getUniformLocation(const std::string &name) const;

        int getAttributeLocation(const std::string &name) const;

    private:
        GLuint m_id;
        std::vector<std::unique_ptr<ShaderGL>> m_shaders;
    };
} // namespace XE

#endif
