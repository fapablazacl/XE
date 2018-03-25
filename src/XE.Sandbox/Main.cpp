
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
        std::unique_ptr<XE::Buffer> coordBuffer = std::make_unique<XE::Graphics::GL::BufferGL> (
            XE::Graphics::BufferType::Vertex, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::coordData.size() * (int)sizeof(XE::Math::Vector3f), 
            XE::Sandbox::Assets::coordData.data()
        );

        std::unique_ptr<XE::Buffer> colorBuffer = std::make_unique<XE::Graphics::GL::BufferGL> (
            XE::Graphics::BufferType::Vertex, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::colorData.size() * (int)sizeof(XE::Math::Vector4f), 
            XE::Sandbox::Assets::colorData.data()
        );

        // create the index buffer
        std::unique_ptr<XE::Buffer> indexBuffer = std::make_unique<XE::Graphics::GL::BufferGL> (
            XE::Graphics::BufferType::Index, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::indexData.size() * (int)sizeof(int), 
            XE::Sandbox::Assets::indexData.data()
        );

        // create the geometry subset
        XE::Graphics::SubsetDescriptor subsetDescriptor;
        
        subsetDescriptor.attributes = {
            {"vertCoord", XE::DataType::Float32, 3}, 
            {"vertColor", XE::DataType::Float32, 4}
        };
        
        subsetDescriptor.buffers.push_back(std::move(coordBuffer));
        subsetDescriptor.buffers.push_back(std::move(colorBuffer));

        subsetDescriptor.bufferMapping = {
            {"vertCoord", 0}, {"vertColor", 1}
        };

        subsetDescriptor.indexBuffer = std::move(indexBuffer);
        subsetDescriptor.indexType = XE::DataType::UInt32;

        XE::Graphics::GL::SubsetGL subset {subsetDescriptor};
        
        while (!::glfwWindowShouldClose(window)) {
            ::glfwPollEvents();
            ::glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            ::glClear(GL_COLOR_BUFFER_BIT);

            ::glBindVertexArray(subset.GetID());
            ::glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, nullptr);

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
