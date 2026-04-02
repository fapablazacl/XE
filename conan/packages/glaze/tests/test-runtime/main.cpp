#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

static const char* VERT_SRC = R"glsl(
#version 330 core
layout(location = 0) in vec2 position;
uniform float angle;
void main() {
    float c = cos(angle);
    float s = sin(angle);
    vec2 r = vec2(c * position.x - s * position.y,
                  s * position.x + c * position.y);
    gl_Position = vec4(r, 0.0, 1.0);
}
)glsl";

static const char* FRAG_SRC = R"glsl(
#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)glsl";

static gl::Shader compileShader(gl::ShaderType type, const char* src) {
    gl::Shader shader(gl::createShader(type));
    gl::shaderSource(shader, 1, &src, nullptr);
    gl::compileShader(shader);

    GLint ok = 0;
    gl::getShaderiv(shader, gl::ShaderParameterName::eCompileStatus, &ok);
    if (!ok) {
        GLint logLen = 0;
        gl::getShaderiv(shader, gl::ShaderParameterName::eInfoLogLength, &logLen);
        std::cerr << "Shader compile error:\n" << gl::getShaderInfoLog(shader, logLen) << std::endl;
        std::exit(1);
    }
    return shader;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "glaze — spinning triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glazeLoadFunctions(glfwGetProcAddress);

    // Geometry — equilateral triangle centred at origin
    static const float verts[] = {
         0.0f,  0.5f,
        -0.433f, -0.25f,
         0.433f, -0.25f,
    };

    gl::VertexArray vao;
    gl::BufferId vbo;
    gl::genVertexArrays(1, &vao);
    gl::genBuffers(1, &vbo);

    gl::bindVertexArray(vao);
    gl::bindBuffer(gl::BufferTargetARB::eArray, vbo);
    gl::bufferData(gl::BufferTargetARB::eArray, sizeof(verts), verts, gl::BufferUsageARB::eStaticDraw);
    gl::vertexAttribPointer(0, 2, gl::VertexAttribPointerType::eFloat, GL_FALSE, 0, nullptr);
    gl::enableVertexAttribArray(0);
    gl::bindVertexArray(gl::VertexArray{});

    // Shader program
    gl::Shader vert = compileShader(gl::ShaderType::eVertex,   VERT_SRC);
    gl::Shader frag = compileShader(gl::ShaderType::eFragment, FRAG_SRC);

    gl::Program prog(gl::createProgram());
    gl::attachShader(prog, vert);
    gl::attachShader(prog, frag);
    gl::linkProgram(prog);

    GLint ok = 0;
    gl::getProgramiv(prog, gl::ProgramPropertyARB::eLinkStatus, &ok);
    if (!ok) {
        GLint logLen = 0;
        gl::getProgramiv(prog, gl::ProgramPropertyARB::eInfoLogLength, &logLen);
        std::cerr << "Link error:\n" << gl::getProgramInfoLog(prog, logLen) << std::endl;
        return -1;
    }

    gl::deleteShader(vert);
    gl::deleteShader(frag);

    GLint angleLoc = gl::getUniformLocation(prog, "angle");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        
        gl::viewport(0, 0, w, h);

        gl::clearColor(0.15f, 0.15f, 0.2f, 1.0f);
        gl::clear(gl::ClearBufferMask::eColorBit);

        gl::useProgram(prog);
        gl::uniform1f(angleLoc, static_cast<float>(glfwGetTime()));

        gl::bindVertexArray(vao);
        gl::drawArrays(gl::PrimitiveType::eTriangles, 0, 3);
        gl::bindVertexArray(gl::VertexArray{});

        glfwSwapBuffers(window);
    }

    gl::deleteVertexArrays(1, &vao);
    gl::deleteBuffers(1, &vbo);
    gl::deleteProgram(prog);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
