
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL/ProgramGL.hpp>
#include <XE/Graphics/GL/BufferGL.hpp>
#include <XE/Graphics/GL/SubsetGL.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Assets.hpp"

int main(int argc, char **argv) {
    try {
        ::glfwInit();
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
        const std::vector<std::tuple<XE::Graphics::ShaderType, std::string>> sources = {
            {XE::Graphics::ShaderType::Vertex, std::string{XE::Sandbox::Assets::vertexShader}},
            {XE::Graphics::ShaderType::Fragment, std::string{XE::Sandbox::Assets::fragmentShader}}
        };

        XE::Graphics::GL::ProgramGL program{sources};

        ::glUseProgram(program.GetID());

        // create the vertex buffer
        XE::Graphics::GL::BufferGL vertexBuffer {
            XE::Graphics::BufferType::Vertex, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::triangleVertices.size() * (int)sizeof(XE::Graphics::Vertex), 
            XE::Sandbox::Assets::triangleVertices.data()
        };

        // create the index buffer
        XE::Graphics::GL::BufferGL indexBuffer {
            XE::Graphics::BufferType::Index, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::triangleIndices.size() * (int)sizeof(int), 
            XE::Sandbox::Assets::triangleIndices.data()
        };

        // create the geometry subset


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

    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
