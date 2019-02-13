
#ifndef __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__
#define __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__

#include <XE/Graphics/GraphicsDevice.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace XE::Input {
    class InputManager;
}

namespace XE {
    class ProgramGL;

    class GraphicsDeviceGL : public GraphicsDevice {
    public:
        GraphicsDeviceGL();

        virtual ~GraphicsDeviceGL();
        
        virtual Window* GetWindow() const override;

        virtual InputManager* GetInputManager() override;

        virtual std::unique_ptr<Subset> CreateSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) override;
        
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferDescriptor &bufferDescriptor) override;
        
        virtual std::unique_ptr<Texture2D> CreateTexture2D(const PixelFormat format, const XE::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual std::unique_ptr<Texture3D> CreateTexture3D(const PixelFormat format, const XE::Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual std::unique_ptr<Texture2DArray> CreateTexture2DArray(const PixelFormat format, const XE::Vector2i &size, const int count) override;
        
        virtual std::unique_ptr<TextureCubeMap> CreateTextureCubeMap(const PixelFormat format, const XE::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) override;
        
        virtual std::unique_ptr<Program> CreateProgram(const ProgramDescriptor &programDescriptor) override;
        
        virtual void SetMaterial(const Material *material) override;

        virtual const Material* GetMaterial() const override;

        virtual void SetProgram(const Program *program) override;

        virtual const Program* GetProgram() const override;

        virtual void ApplyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) override;

        virtual void ApplyUniform(const Uniform *uniform, const int count, const std::byte *data) override;

        virtual void BeginFrame(const ClearFlags flags, const XE::Vector4f &color, const float depth, const int stencil) override;
        
        virtual void Draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) override;

        virtual void EndFrame() override;

        virtual void SetViewport(const Viewport &viewport) override;

        virtual Viewport GetViewport() const override;

    private:
        void PreRenderMaterial(const Material *material);

        void PostRenderMaterial(const Material *material);

    private:
        GLFWwindow *m_windowGLFW = nullptr;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<InputManager> m_inputManager;
        const ProgramGL *m_program = nullptr;
        const Material *m_material = nullptr;

        Viewport m_viewport;
    };
}

#endif
