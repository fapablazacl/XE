
#pragma once 

#include <string>
#include <vector>
#include <glad/glad.h>


class RendererGL {
public:
	GLuint createShader(const GLenum type, const std::string &source) const;

	GLuint createProgram(const std::vector<GLuint> &shaders) const;

	GLuint createBuffer(const GLenum target, const GLenum usage, const GLvoid *data, const size_t size) const;
};
