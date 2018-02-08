
#include "GraphicsDeviceGL.hpp"

#include "BufferGL.hpp"
#include "Texture2DGL.hpp"
#include "ProgramGL.hpp"

#include <XE/Graphics/Subset.hpp>
#include <XE/Graphics/Texture3D.hpp>
#include <XE/Graphics/Texture2DArray.hpp>
#include <XE/Graphics/TextureCubeMap.hpp>

namespace XE::Graphics::GL {
    GraphicsDeviceGL::GraphicsDeviceGL() {}

    GraphicsDeviceGL::~GraphicsDeviceGL() {}
        
    XE::Input::InputManager* GraphicsDeviceGL::GetInputManager() {
        return nullptr;
    }

    std::unique_ptr<Subset> GraphicsDeviceGL::CreateSubset(const VertexLayout &format, std::vector<std::unique_ptr<Buffer>> buffers, const DataType indexType, std::unique_ptr<Buffer> indexBuffer) {
        return std::unique_ptr<Subset>();
    }
        
    std::unique_ptr<Buffer> GraphicsDeviceGL::CreateBuffer(const BufferType bufferType, const int size, const void *data) {
        return std::make_unique<BufferGL>(bufferType, size, data);
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
        
    std::unique_ptr<Program> GraphicsDeviceGL::CreateProgram(const std::vector<std::tuple<ShaderType, std::string>> &sources) {
        return std::make_unique<ProgramGL>(sources);
    }
        
    void GraphicsDeviceGL::SetProgram(const Program *program) {
    }
        
    const Program* GraphicsDeviceGL::GetProgram() const {
        return nullptr;
    }
        
    void GraphicsDeviceGL::Draw(const Subset *subset, const std::vector<DrawEnvelope> &envelopes) {
    }
        
    void GraphicsDeviceGL::BeginFrame(const ClearFlags flags, const XE::Math::Vector4f &color, const float depth, const int stencil) {
    }
        
    void GraphicsDeviceGL::EndFrame() {
    }
}
