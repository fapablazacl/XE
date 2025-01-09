
#pragma once 

#include <string>
#include <vector>
#include <glad/glad.h>

#include "Types.h"

struct RendererInfo {
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string shadingLanguageVersion;
};

namespace xe::gl {
    class RendererGL {
    public:
        RendererGL();

        Shader createShader(const GLenum type, const std::string &source) const;

        Program createProgram(const std::vector<Shader> &shaders) const;

        Buffer createBuffer(const GLenum target, const GLenum usage, const GLvoid *data, const GLsizei size) const;

        RendererInfo getInfo() const;
    };
}
