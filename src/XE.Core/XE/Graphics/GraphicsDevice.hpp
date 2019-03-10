
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

    struct Viewport;
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

    /**
     * @brief The 'Window' that is beign used as the render target for the graphics device contents
     */
    class XE_API Window {
    public:
        virtual ~Window() {}

        /**
         * @brief Get the current size of the Window, in Pixels, where the X and Y fields correspond to the Width and Height of the Window, respectively
         */
        virtual Vector2i getSize() const = 0;
    };

    /**
     * @brief Graphics API abstraction
     */
    class XE_API GraphicsDevice {
    public:
        virtual ~GraphicsDevice();

        virtual InputManager* getInputManager() = 0;

        virtual Window* getWindow() const = 0;

        /**
         * @brief Create a geometry subset composed of many geometry and indexation buffers
         * @param buffers The buffers that have data
         * @param bufferMapping The mapping between the buffers and the vertex attributes
         * @param indexBuffer The subset geometry index buffer
         * @todo Add support for packed vertex structures
         */
        virtual std::unique_ptr<Subset> createSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) = 0;
        
        virtual std::unique_ptr<Buffer> createBuffer(const BufferDescriptor &bufferDescriptor) = 0;
        
        virtual std::unique_ptr<Texture2D> createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture3D> createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture2DArray> createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) = 0;
        
        virtual std::unique_ptr<TextureCubeMap> createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) = 0;
        
        virtual std::unique_ptr<Program> createProgram(const ProgramDescriptor &programDescriptor) = 0;
        
        virtual void setViewport(const Viewport &viewport) = 0;

        virtual Viewport getViewport() const = 0;

        virtual void setMaterial(const Material *material) = 0;
        
        virtual const Material* getMaterial() const = 0;

        virtual void setProgram(const Program *program) = 0;

        virtual const Program* getProgram() const = 0;

        virtual void applyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) = 0;

        virtual void applyUniform(const Uniform *uniform, const int count, const std::byte *data) = 0;

        virtual void draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) = 0;
        
        virtual void beginFrame(const ClearFlags flags, const Vector4f &color, const float depth, const int stencil) = 0;
        
        virtual void endFrame() = 0;
    };
}

#endif
