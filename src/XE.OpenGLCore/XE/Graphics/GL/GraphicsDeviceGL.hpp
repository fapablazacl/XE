
#ifndef __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__
#define __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__

#include <XE/Graphics/GraphicsDevice.hpp>

namespace XE::Graphics::GL {
    class GraphicsDeviceGL : public GraphicsDevice {
    public:
        GraphicsDeviceGL();

        virtual ~GraphicsDeviceGL();
        
        virtual XE::Input::InputManager* GetInputManager() override;

        virtual std::unique_ptr<Subset> CreateSubset(const VertexLayout &format, std::vector<std::unique_ptr<Buffer>> buffers, const DataType indexType, std::unique_ptr<Buffer> indexBuffer) override;
        
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferType bufferType, const int size, const void *data=nullptr) override;
        
        virtual std::unique_ptr<Texture2D> CreateTexture2D(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual std::unique_ptr<Texture3D> CreateTexture3D(const PixelFormat format, const XE::Math::Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual std::unique_ptr<Texture2DArray> CreateTexture2DArray(const PixelFormat format, const XE::Math::Vector2i &size, const int count, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) override;
        
        virtual std::unique_ptr<TextureCubeMap> CreateTextureCubeMap(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) override;
        
        virtual std::unique_ptr<Program> CreateProgram(const std::vector<std::tuple<ShaderType, std::string>> &sources) override;
        
        virtual void SetProgram(const Program *program) override;
        
        virtual const Program* GetProgram() const override;
        
        virtual void Draw(const Subset *subset, const std::vector<DrawEnvelope> &envelopes) override;
        
        virtual void BeginFrame(const ClearFlags flags, const XE::Math::Vector4f &color, const float depth, const int stencil) override;
        
        virtual void EndFrame() override;
    };
}

#endif
