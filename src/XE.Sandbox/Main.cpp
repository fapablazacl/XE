
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
    }

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
