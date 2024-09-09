
#pragma once 

#include <string>
#include <vector>
#include <glad/glad.h>

struct RendererInfo {
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string shadingLanguageVersion;
};


class RendererGL {
public:
    RendererGL();

	GLuint createShader(const GLenum type, const std::string &source) const;

	GLuint createProgram(const std::vector<GLuint> &shaders) const;

	GLuint createBuffer(const GLenum target, const GLenum usage, const GLvoid *data, const size_t size) const;

    RendererInfo getInfo() const;
};
