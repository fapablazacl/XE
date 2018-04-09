
#include "SandboxApp.hpp"
#include "Assets.hpp"

#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Math.hpp>
#include <XE/Input/InputManager.hpp>
#include <XE/Input/DeviceStatus.hpp>
#include <XE/Input/InputCode.hpp>
#include <XE/Graphics/GL/GraphicsDeviceGL.hpp>
#include <XE/Graphics/GL/ProgramGL.hpp>
#include <XE/Graphics/GL/BufferGL.hpp>
#include <XE/Graphics/GL/SubsetGL.hpp>

#include <iostream>

namespace XE::Sandbox {
    class SandboxApp : public Application {
    public:
        explicit SandboxApp(const std::vector<std::string> &args) {}
        virtual ~SandboxApp() {}

        virtual void Initialize() override {
            std::cout << "Initializing Engine ..." << std::endl;
            m_graphicsDevice = std::make_unique<XE::Graphics::GL::GraphicsDeviceGL>();
            m_inputManager = m_graphicsDevice->GetInputManager();

            std::cout << "Loading assets ..." << std::endl;
            this->InitializeShaders();
            this->InitializeGeometry();
        }

        virtual void Update(const float seconds) override {
            m_inputManager->Poll();
            
            const XE::Input::KeyboardStatus keyboardStatus = m_inputManager->GetKeyboardStatus();

            if (keyboardStatus.GetState(XE::Input::KeyCode::KeyEsc) == XE::Input::BinaryState::Press) {
                m_shouldClose = true;
            }

            // update animation
            m_angle += 0.333333f;

            // update camera position
            const bool moveForward = keyboardStatus.GetState(XE::Input::KeyCode::KeyUp) == XE::Input::BinaryState::Press;
            const bool moveBackward = keyboardStatus.GetState(XE::Input::KeyCode::KeyDown) == XE::Input::BinaryState::Press;

            const bool turnLeft = keyboardStatus.GetState(XE::Input::KeyCode::KeyLeft) == XE::Input::BinaryState::Press;
            const bool turnRight = keyboardStatus.GetState(XE::Input::KeyCode::KeyRight) == XE::Input::BinaryState::Press;

            // camera turning
            const auto cameraDirection = XE::Math::Normalize(m_cameraLookAt - m_cameraPosition);
            float turnSpeed = 0.0f;
            if (turnLeft) {
                turnSpeed = -XE::Math::Radians(0.25f);
            } else if (turnRight) {
                turnSpeed = XE::Math::Radians(0.25f);
            }

            const auto cdt = XE::Math::Matrix4f::Rotate(turnSpeed, m_cameraUp) * XE::Math::Vector4f(cameraDirection, 0.0f);
            
            m_cameraLookAt = m_cameraPosition + XE::Math::Vector3f{cdt.X, cdt.Y, cdt.Z} * XE::Math::Magnitude(cameraDirection);

            // camera movement
            const auto cameraSpeed = 0.0025f;
            const auto cameraDisplacement = cameraDirection * cameraSpeed * seconds;
            const auto cameraSide = XE::Math::Normalize(XE::Math::Cross(cameraDirection, m_cameraUp));
            
            if (moveForward) {
                m_cameraPosition += cameraDisplacement;
                m_cameraLookAt += cameraDisplacement;
            } else if (moveBackward) {
                m_cameraPosition -= cameraDisplacement;
                m_cameraLookAt -= cameraDisplacement;
            }
        }

        /*
        void RenderMatrices_() {
            XE::Graphics::UniformMatrix matrixLayout[] = {
                { "m_model", XE::DataType::Float32, 4, 4, 1 }, 
                { "m_view",  XE::DataType::Float32, 4, 4, 1 }, 
                { "m_proj",  XE::DataType::Float32, 4, 4, 1 },
            };

            const int matrixCount = 3;
            
            XE::Math::Matrix4f matrices[matrixCount] = {
                XE::Math::Matrix4f::Identity(),
                // XE::Math::Matrix4f::RotateY(XE::Math::Radians(m_angle)),

                // XE::Math::Matrix4f::Identity(),
                // XE::Math::Matrix4f::LookAt(m_cameraPosition, m_cameraLookAt, m_cameraUp),
                XE::Math::Matrix4f::Translate(-m_cameraPosition),

                // XE::Math::Matrix4f::Identity(),
                XE::Math::Matrix4f::Perspective(m_cameraFov, 4.0f/3.0f, m_cameraZNear, m_cameraZFar),
            };

            m_graphicsDevice->ApplyUniform(matrixLayout, matrixCount, (const std::byte*)&matrices);
        }
        */
        void RenderMatrices() {
            const XE::Graphics::UniformMatrix matrixLayout = { "m_mvp", XE::DataType::Float32, 4, 4, 1 };

            const XE::Math::Matrix4f modelViewProj = XE::Math::Transpose (
                XE::Math::Matrix4f::Perspective(m_cameraFov, m_cameraAspect, m_cameraZNear, m_cameraZFar) * 
                XE::Math::Matrix4f::LookAt(m_cameraPosition, m_cameraLookAt, m_cameraUp) * 
                XE::Math::Matrix4f::RotateY(XE::Math::Radians(m_angle)) 
            );

            m_graphicsDevice->ApplyUniform(&matrixLayout, 1, (const std::byte*)&modelViewProj);
        }

        virtual void Render() override {
            m_graphicsDevice->BeginFrame(XE::Graphics::ClearFlags::All, {0.2f, 0.2f, 0.2f, 1.0f}, 0.0f, 0);
            m_graphicsDevice->SetProgram(m_program.get());

            this->RenderMatrices();

            m_graphicsDevice->SetMaterial(m_material.get());

            XE::Graphics::SubsetEnvelope envelope = {
                nullptr, XE::Graphics::PrimitiveType::TriangleStrip, 0, 3
            };
            m_graphicsDevice->Draw(m_subset.get(), &envelope, 1);
            m_graphicsDevice->EndFrame();
        }

        virtual bool ShouldClose() const override {
            return m_shouldClose;
        }

    private:
        void InitializeShaders() {
            // setup program shader
            const XE::Graphics::ProgramDescriptor programDescriptor = {
                {
                    {XE::Graphics::ShaderType::Vertex, std::string{XE::Sandbox::Assets::vertexShader}},
                    {XE::Graphics::ShaderType::Fragment, std::string{XE::Sandbox::Assets::fragmentShader}}
                }
            };

            m_program = m_graphicsDevice->CreateProgram(programDescriptor);
        }

        void InitializeGeometry() {
            // create the vertex buffer
            XE::Graphics::BufferDescriptor coordBufferDescriptor = {
                XE::Graphics::BufferType::Vertex, 
                XE::Graphics::BufferUsage::Copy, 
                XE::Graphics::BufferAccess::Static, 
                (int)XE::Sandbox::Assets::coordData.size() * (int)sizeof(XE::Math::Vector3f), 
                (const std::byte*) XE::Sandbox::Assets::coordData.data()
            };

            auto coordBuffer = m_graphicsDevice->CreateBuffer(coordBufferDescriptor);

            XE::Graphics::BufferDescriptor colorBufferDescriptor = {
                XE::Graphics::BufferType::Vertex, 
                XE::Graphics::BufferUsage::Copy, 
                XE::Graphics::BufferAccess::Static, 
                (int)XE::Sandbox::Assets::colorData.size() * (int)sizeof(XE::Math::Vector4f), 
                (const std::byte*) XE::Sandbox::Assets::colorData.data()
            };

            auto colorBuffer = m_graphicsDevice->CreateBuffer(colorBufferDescriptor);

            // create the index buffer
            XE::Graphics::BufferDescriptor indexBufferDescriptor = {
                XE::Graphics::BufferType::Index, 
                XE::Graphics::BufferUsage::Copy, 
                XE::Graphics::BufferAccess::Static, 
                (int)XE::Sandbox::Assets::indexData.size() * (int)sizeof(int), 
                (const std::byte*) XE::Sandbox::Assets::indexData.data()
            };

            auto indexBuffer = m_graphicsDevice->CreateBuffer(indexBufferDescriptor);

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

            m_subset = m_graphicsDevice->CreateSubset(subsetDescriptor, std::move(buffers), bufferMapping, std::move(indexBuffer));

            m_material = std::make_unique<XE::Graphics::Material>();
        }

    private:
        std::unique_ptr<XE::Graphics::Program> m_program;
        std::unique_ptr<XE::Graphics::GraphicsDevice> m_graphicsDevice;
        std::unique_ptr<XE::Graphics::Subset> m_subset;
        std::unique_ptr<XE::Graphics::Material> m_material;
        XE::Input::InputManager *m_inputManager = nullptr;

        bool m_shouldClose = false;

        float m_angle = 0.0f;

        XE::Math::Vector3f m_cameraPosition = {0.0f, 0.0f, 5.0f};
        XE::Math::Vector3f m_cameraLookAt = {0.0f, 0.0f, 0.0f};
        XE::Math::Vector3f m_cameraUp = {0.0f, 1.0f, 0.0f};

        float m_cameraFov = XE::Math::Radians(60.0f);
        float m_cameraAspect = 640.0f / 480.0f;
        float m_cameraZNear = 0.1f;
        float m_cameraZFar = 1000.0f;
    };

    std::unique_ptr<Application> Application::Create(const std::vector<std::string> &args) {
        return std::make_unique<SandboxApp>(args);
    }
}
