
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Assets.hpp"

int main(int argc, char **argv) {
    ::glfwInit();
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    ::glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, 1);
    ::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    ::glfwWindowHint(GLFW_DEPTH_BITS, 24);
    ::glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    ::glfwWindowHint(GLFW_RED_BITS, 8);
    ::glfwWindowHint(GLFW_GREEN_BITS, 8);
    ::glfwWindowHint(GLFW_BLUE_BITS, 8);
    ::glfwWindowHint(GLFW_ALPHA_BITS, 0);

    GLFWwindow *window = ::glfwCreateWindow(1200, 800, "Test", nullptr, nullptr);

    ::glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Failed to load OpenGL extensions" << std::endl;
        return 1;
    }

    // setup vertex shader
    GLuint vertexShader = ::glCreateShader(GL_VERTEX_SHADER);

    const char *vertexShaderSource = XE::Sandbox::Assets::vertexShader;
    const int vertexShaderLength = (int)std::strlen(vertexShaderSource);

    ::glShaderSource(vertexShader, 1, &vertexShaderSource, &vertexShaderLength);
    ::glCompileShader(vertexShader);

    // setup fragment shader
    GLuint fragmentShader = ::glCreateShader(GL_FRAGMENT_SHADER);

    const char *fragmentShaderSource = XE::Sandbox::Assets::fragmentShader;
    const int fragmentShaderLength = (int)std::strlen(fragmentShaderSource);

    ::glShaderSource(fragmentShader, 1, &fragmentShaderSource, &vertexShaderLength);
    ::glCompileShader(fragmentShader);
    
    // setup shader program
    GLuint shaderProgram = ::glCreateProgram();
    ::glAttachShader(shaderProgram, vertexShader);
    ::glAttachShader(shaderProgram, fragmentShader);
    ::glLinkProgram(shaderProgram);

    while (!::glfwWindowShouldClose(window)) {
        ::glfwPollEvents();
        ::glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT);

        ::glFlush();
        ::glfwSwapBuffers(window);
    }

    ::glfwMakeContextCurrent(nullptr);
    ::glfwDestroyWindow(window);
    ::glfwTerminate();

    return 0;
}
