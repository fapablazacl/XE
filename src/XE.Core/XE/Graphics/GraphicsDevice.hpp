
#ifndef __XE_GRAPHICS_GRAPHICSDEVICE_HPP__
#define __XE_GRAPHICS_GRAPHICSDEVICE_HPP__

#include <vector>
#include <memory>
#include <map>
#include <XE/DataType.hpp>
#include <XE/Predef.hpp>
#include <XE/Math/Vector.hpp>

namespace XE {
    class Buffer;
    class InputManager;

    /**
     * @brief Specify wich buffer will be used to clear
     */
    enum class ClearFlags {
        Color = 0x01,
        Depth = 0x02,
        Stencil = 0x04,
        ColorDepth = Color | Depth,
        All = Color | Depth | Stencil
    };
    
    inline bool operator&(const ClearFlags flags, const ClearFlags value) {
        int nflags = static_cast<int>(flags);
        int nvalue = static_cast<int>(value);
        
        if (nflags & nvalue) {
            return true;
        } else {
            return false;
        }
    }
    
    enum class PixelFormat;
    enum class ShaderType;

    struct BufferDescriptor;
    struct SubsetEnvelope;
    struct VertexLayout;
    struct Uniform;
    struct UniformMatrix;
    
    class Subset;
    class Texture2D;
    class Texture3D;
    class Texture2DArray;
    class TextureCubeMap;
    class Program;
    class Material;

    struct SubsetDescriptor;
    struct ProgramDescriptor;

    struct Viewport {
        XE::Vector2i position;
        XE::Vector2i size;
    };

    /**
     * @brief The 'Window' that is beign used as the render target for the graphics device contents
     */
    class XE_API Window {
    public:
        virtual ~Window() {}

        /**
         * @brief Get the current size of the Window, in Pixels, where the X and Y fields correspond to the Width and Height of the Window, respectively
         */
        virtual XE::Vector2i GetSize() const = 0;
    };

    /**
     * @brief Graphics API abstraction
     */
    class XE_API GraphicsDevice {
    public:
        virtual ~GraphicsDevice();

        virtual InputManager* GetInputManager() = 0;

        virtual Window* GetWindow() const = 0;

        /**
         * @brief Create a geometry subset composed of many geometry and indexation buffers
         * @param buffers The buffers that have data
         * @param bufferMapping The mapping between the buffers and the vertex attributes
         * @param indexBuffer The subset geometry index buffer
         * @todo Add support for packed vertex structures
         */
        virtual std::unique_ptr<Subset> CreateSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) = 0;
        
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferDescriptor &bufferDescriptor) = 0;
        
        virtual std::unique_ptr<Texture2D> CreateTexture2D(const PixelFormat format, const XE::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture3D> CreateTexture3D(const PixelFormat format, const XE::Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture2DArray> CreateTexture2DArray(const PixelFormat format, const XE::Vector2i &size, const int count) = 0;
        
        virtual std::unique_ptr<TextureCubeMap> CreateTextureCubeMap(const PixelFormat format, const XE::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) = 0;
        
        virtual std::unique_ptr<Program> CreateProgram(const ProgramDescriptor &programDescriptor) = 0;
        
        virtual void SetViewport(const Viewport &viewport) = 0;

        virtual Viewport GetViewport() const = 0;

        virtual void SetMaterial(const Material *material) = 0;
        
        virtual const Material* GetMaterial() const = 0;

        virtual void SetProgram(const Program *program) = 0;

        virtual const Program* GetProgram() const = 0;

        virtual void ApplyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) = 0;

        virtual void ApplyUniform(const Uniform *uniform, const int count, const std::byte *data) = 0;

        virtual void Draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) = 0;
        
        virtual void BeginFrame(const ClearFlags flags, const XE::Vector4f &color, const float depth, const int stencil) = 0;
        
        virtual void EndFrame() = 0;
    };
}

#endif
