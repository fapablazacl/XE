
#include <array>
#include <string>
#include <iostream>
#include <map>
#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Math.hpp>
#include <XE/Input/InputManager.hpp>
#include <XE/Input/DeviceStatus.hpp>
#include <XE/Input/InputCode.hpp>
#include <XE/Graphics/Uniform.hpp>
#include <XE/Graphics/GL/GraphicsDeviceGL.hpp>
#include <XE/Graphics/GL/ProgramGL.hpp>
#include <XE/Graphics/GL/BufferGL.hpp>
#include <XE/Graphics/GL/SubsetGL.hpp>

#include "Assets.hpp"

int main(int argc, char **argv) {
    try {
        std::cout << "Initializing Engine ..." << std::endl;
        auto graphicsDevice = std::make_unique<XE::Graphics::GL::GraphicsDeviceGL>();
        auto inputManager = graphicsDevice->GetInputManager();

        std::cout << "Loading assets ..." << std::endl;
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

        std::cout << "Entering main loop" << std::endl;

        float angle = 0.0f;

        while (true) {
            inputManager->Poll();
            
            const XE::Input::KeyboardStatus keyboardStatus = inputManager->GetKeyboardStatus();

            if (keyboardStatus.GetState(XE::Input::KeyCode::KeyEsc) == XE::Input::BinaryState::Press) {
                break;
            }

            angle += 0.333333f;

            graphicsDevice->BeginFrame(XE::Graphics::ClearFlags::All, {0.2f, 0.2f, 0.2f, 1.0f}, 0.0f, 0);
            graphicsDevice->SetProgram(program.get());

            XE::Graphics::UniformMatrix matrixLayout[] = {
                {
                    "m_model", XE::DataType::Float32, 
                    4, 4, 1
                }, 
                {
                    "m_view", XE::DataType::Float32, 
                    4, 4, 1
                }, 
                {
                    "m_proj", XE::DataType::Float32, 
                    4, 4, 1
                },
            };

            XE::Math::Matrix4f matrices[] = {
                XE::Math::Matrix4f::RotateX(XE::Math::Radians(angle)) * 
                XE::Math::Matrix4f::RotateY(XE::Math::Radians(angle)) * 
                XE::Math::Matrix4f::RotateZ(XE::Math::Radians(angle)),
                XE::Math::Matrix4f::Identity(),
                XE::Math::Matrix4f::Identity(),
            };

            graphicsDevice->ApplyUniform(matrixLayout, 3, (const std::byte*)&matrices);

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
