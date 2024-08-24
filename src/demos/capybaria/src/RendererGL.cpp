
#include "RendererGL.h"

#include <iostream>

constexpr size_t INFO_LOG_BUFFER_SIZE = 4096;


GLuint RendererGL::createShader(const GLenum type, const std::string& source) const {
    if (source.empty()) {
        std::cerr << "Error while creating shader: Non-empty string expected" << std::endl;
        return 0;
    }

    const auto shaderId = glCreateShader(type);

    const GLchar *const glsl = source.c_str();
    auto size = static_cast<GLsizei>(source.size());
    glShaderSource(shaderId, 1, &glsl, &size);
    glCompileShader(shaderId);

    // check for errors
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

    if (status == static_cast<GLint>(GL_FALSE)) {
        std::cerr << "Error while creating shader " << type << ": ";

        char msg[INFO_LOG_BUFFER_SIZE] = {};
        glGetShaderInfoLog(shaderId, INFO_LOG_BUFFER_SIZE, nullptr, msg);

        std::cerr << msg << std::endl;

        return 0;
    }

    return shaderId;
}


GLuint RendererGL::createProgram(const std::vector<GLuint> &shaderIds) const {
    const auto programId = glCreateProgram();

    for (auto &shaderId : shaderIds) {
        if (shaderId == 0) {
            glDeleteProgram(programId);
            return 0;
        }

        glAttachShader(programId, shaderId);
    }

    glLinkProgram(programId);

    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);

    if (status == static_cast<GLint>(GL_FALSE)) {
        std::cerr << "Error while creating program: ";

        char msg[INFO_LOG_BUFFER_SIZE] = {};
        glGetProgramInfoLog(programId, INFO_LOG_BUFFER_SIZE, nullptr, msg);

        std::cerr << msg << std::endl;

        return 0;
    }

    return programId;
}


GLuint RendererGL::createBuffer(const GLenum target, const GLenum usage, const GLvoid *data, const size_t size) const {
    GLuint bufferId = 0;

    glGenBuffers(1, &bufferId);
    glBindBuffer(target, bufferId);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);

    return bufferId;
}
