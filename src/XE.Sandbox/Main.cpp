
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL/GraphicsDeviceGL.hpp>
#include <XE/Graphics/GL/ProgramGL.hpp>
#include <XE/Graphics/GL/BufferGL.hpp>
#include <XE/Graphics/GL/SubsetGL.hpp>

#include "Assets.hpp"

int main(int argc, char **argv) {
    try {
        auto graphicsDevice = std::make_unique<XE::Graphics::GL::GraphicsDeviceGL>();

        // setup vertex shader
        const XE::Graphics::ProgramDescriptor programDescriptor = {
            {
                {XE::Graphics::ShaderType::Vertex, std::string{XE::Sandbox::Assets::vertexShader}},
                {XE::Graphics::ShaderType::Fragment, std::string{XE::Sandbox::Assets::fragmentShader}}
            }
        };

        auto program = graphicsDevice->CreateProgram(programDescriptor);

        // create the vertex buffer
        XE::Graphics::BufferDescriptor coordBufferDescriptor = {
            XE::Graphics::BufferType::Vertex, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::coordData.size() * (int)sizeof(XE::Math::Vector3f), 
            (const std::byte*) XE::Sandbox::Assets::coordData.data()
        };

        auto coordBuffer = graphicsDevice->CreateBuffer(coordBufferDescriptor);

        XE::Graphics::BufferDescriptor colorBufferDescriptor = {
            XE::Graphics::BufferType::Vertex, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::colorData.size() * (int)sizeof(XE::Math::Vector4f), 
            (const std::byte*) XE::Sandbox::Assets::colorData.data()
        };

        auto colorBuffer = graphicsDevice->CreateBuffer(colorBufferDescriptor);

        // create the index buffer
        XE::Graphics::BufferDescriptor indexBufferDescriptor = {
            XE::Graphics::BufferType::Index, 
            XE::Graphics::BufferUsage::Copy, 
            XE::Graphics::BufferAccess::Static, 
            (int)XE::Sandbox::Assets::indexData.size() * (int)sizeof(int), 
            (const std::byte*) XE::Sandbox::Assets::indexData.data()
        };

        auto indexBuffer = graphicsDevice->CreateBuffer(indexBufferDescriptor);

        // create the geometry subset
        XE::Graphics::SubsetDescriptor subsetDescriptor;
        
        subsetDescriptor.attributes = {
            {"vertCoord", XE::DataType::Float32, 3}, 
            {"vertColor", XE::DataType::Float32, 4}
        };

        subsetDescriptor.indexType = XE::DataType::UInt32;

        std::vector<std::unique_ptr<XE::Buffer>> buffers;
        buffers.push_back(std::move(coordBuffer));
        buffers.push_back(std::move(colorBuffer));

        std::map<std::string, int> bufferMapping = {
            {"vertCoord", 0}, {"vertColor", 1}
        };

        auto subset = graphicsDevice->CreateSubset(subsetDescriptor, std::move(buffers), bufferMapping, std::move(indexBuffer));

        while (true) {
            graphicsDevice->BeginFrame(XE::Graphics::ClearFlags::All, {0.2f, 0.2f, 0.2f, 1.0f}, 0.0f, 0);
            graphicsDevice->SetProgram(program.get());

            XE::Graphics::SubsetEnvelope envelope = {
                nullptr, XE::Graphics::PrimitiveType::TriangleList, 0, 3
            };

            graphicsDevice->Draw(subset.get(), &envelope, 1);
            graphicsDevice->EndFrame();
        }
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
