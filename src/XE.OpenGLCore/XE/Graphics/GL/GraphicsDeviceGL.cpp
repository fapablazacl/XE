
#include "GraphicsDeviceGL.hpp"

#include "Conversion.hpp"
#include "BufferGL.hpp"
#include "SubsetGL.hpp"
#include "Texture2DGL.hpp"
#include "ProgramGL.hpp"

#include <XE/Graphics/Subset.hpp>
#include <XE/Graphics/Texture3D.hpp>
#include <XE/Graphics/Texture2DArray.hpp>
#include <XE/Graphics/TextureCubeMap.hpp>

namespace XE::Graphics::GL {
    GraphicsDeviceGL::GraphicsDeviceGL() {
        ::glfwInit();
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        ::glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, 1);
        ::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
        ::glfwWindowHint(GLFW_DEPTH_BITS, 24);
        ::glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
        ::glfwWindowHint(GLFW_RED_BITS, 8);
        ::glfwWindowHint(GLFW_GREEN_BITS, 8);
        ::glfwWindowHint(GLFW_BLUE_BITS, 8);
        ::glfwWindowHint(GLFW_ALPHA_BITS, 0);

        m_window = ::glfwCreateWindow(1200, 800, "Test", nullptr, nullptr);

        ::glfwMakeContextCurrent(m_window);

        if (!gladLoadGL()) {
            throw std::runtime_error("Failed to load OpenGL extensions");
        }
    }

    GraphicsDeviceGL::~GraphicsDeviceGL() {
        if (m_window) {
            ::glfwMakeContextCurrent(nullptr);
            ::glfwDestroyWindow(m_window);
            ::glfwTerminate();

            m_window = nullptr;
        }
    }
        
    XE::Input::InputManager* GraphicsDeviceGL::GetInputManager() {
        return nullptr;
    }

    std::unique_ptr<Subset> GraphicsDeviceGL::CreateSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) {
        return std::make_unique<SubsetGL>(desc, std::move(buffers), bufferMapping, std::move(indexBuffer));
    }
        
    std::unique_ptr<Buffer> GraphicsDeviceGL::CreateBuffer(const BufferDescriptor &desc) {
        return std::make_unique<BufferGL>(desc);
    }
        
    std::unique_ptr<Texture2D> GraphicsDeviceGL::CreateTexture2D(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::make_unique<Texture2DGL>(format, size, sourceFormat, sourceDataType, sourceData);
    }
        
    std::unique_ptr<Texture3D> GraphicsDeviceGL::CreateTexture3D(const PixelFormat format, const XE::Math::Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::unique_ptr<Texture3D>();
    }
        
    std::unique_ptr<Texture2DArray> GraphicsDeviceGL::CreateTexture2DArray(const PixelFormat format, const XE::Math::Vector2i &size, const int count, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {
        return std::unique_ptr<Texture2DArray>();
    }
        
    std::unique_ptr<TextureCubeMap> GraphicsDeviceGL::CreateTextureCubeMap(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {
        return std::unique_ptr<TextureCubeMap>();
    }
        
    std::unique_ptr<Program> GraphicsDeviceGL::CreateProgram(const ProgramDescriptor &desc) {
        return std::make_unique<ProgramGL>(desc);
    }
    
    void GraphicsDeviceGL::Draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) {
        auto subsetGL = static_cast<const SubsetGL *>(subset);
        auto descriptor = subsetGL->GetDescriptor();
        ::glBindVertexArray(subsetGL->GetID());

        auto indexBuffer = subsetGL->GetIndexBuffer();

        if (!indexBuffer) {
            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = ConvertToGL(env.Primitive);

                ::glDrawArrays(primitiveGL, env.VertexStart, env.VertexCount);
            }
        } else {
            GLenum indexTypeGL = ConvertToGL(descriptor.indexType);

            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = ConvertToGL(env.Primitive);
                
                if (env.VertexStart == 0) {
                    ::glDrawElements(primitiveGL, env.VertexCount, indexTypeGL, nullptr);
                } else {
                    ::glDrawElementsBaseVertex(primitiveGL, env.VertexCount, indexTypeGL, nullptr, env.VertexStart);
                }
            }
        }

        ::glBindVertexArray(0);
    }
        
    void GraphicsDeviceGL::BeginFrame(const ClearFlags flags, const XE::Math::Vector4f &color, const float depth, const int stencil) {
        ::glfwPollEvents();
        ::glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT);
    }
        
    void GraphicsDeviceGL::EndFrame() {
        ::glFlush();
        ::glfwSwapBuffers(m_window);
    }

    void GraphicsDeviceGL::SetMaterial(const Material *material) {

    }

    void GraphicsDeviceGL::SetProgram(const Program *program) {
        auto programGL = static_cast<const ProgramGL *>(program);

        ::glUseProgram(programGL->GetID());
    }
    
    const Program* GraphicsDeviceGL::GetProgram() const {
        return nullptr;
    }

    const Material* GraphicsDeviceGL::GetMaterial() const {
        return nullptr;
    }
}
