
#include "RendererGL.h"

#include <iostream>

constexpr size_t INFO_LOG_BUFFER_SIZE = 4096;


namespace xe::gl {
    RendererGL::RendererGL() {
        const auto info = getInfo();
        std::printf("OpenGL info:\n");
        std::printf("GL_VENDOR: %s\n", info.vendor.c_str());
        std::printf("GL_RENDERER: %s\n", info.renderer.c_str());
        std::printf("GL_VERSION: %s\n", info.version.c_str());
        std::printf("GL_SHADING_LANGUAGE_VERSION: %s\n", info.shadingLanguageVersion.c_str());
    }

    Shader RendererGL::createShader(const GLenum type, const std::string& source) const {
        if (source.empty()) {
            std::cerr << "Error while creating shader: Non-empty string expected" << std::endl;
            return {};
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

            return {};
        }

        return {shaderId};
    }


    Program RendererGL::createProgram(const std::vector<Shader> &shader) const {
        const auto programId = glCreateProgram();

        for (auto &shader : shader) {
            if (shader.value == 0) {
                glDeleteProgram(programId);
                return { 0 };
            }

            glAttachShader(programId, shader.value);
        }

        glLinkProgram(programId);

        GLint status;
        glGetProgramiv(programId, GL_LINK_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            std::cerr << "Error while creating program: ";

            char msg[INFO_LOG_BUFFER_SIZE] = {};
            glGetProgramInfoLog(programId, INFO_LOG_BUFFER_SIZE, nullptr, msg);

            std::cerr << msg << std::endl;

            return {};
        }

        return {programId};
    }


    Buffer RendererGL::createBuffer(const GLenum target, const GLenum usage, const GLvoid *data, const GLsizei size) const {
        GLuint bufferId = 0;

        glGenBuffers(1, &bufferId);
        glBindBuffer(target, bufferId);
        glBufferData(target, size, data, usage);
        glBindBuffer(target, 0);

        return {bufferId};
    }


    RendererInfo RendererGL::getInfo() const {
        RendererInfo info;

        info.vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        info.renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        info.version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        info.shadingLanguageVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        return info;
    }

}
