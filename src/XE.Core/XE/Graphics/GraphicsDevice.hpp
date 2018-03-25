
#ifndef __XE_GRAPHICS_GRAPHICSDEVICE_HPP__
#define __XE_GRAPHICS_GRAPHICSDEVICE_HPP__

#include <vector>
#include <memory>
#include <map>
#include <XE/DataType.hpp>
#include <XE/Math/Vector.hpp>

namespace XE {
    class Buffer;
}

namespace XE::Input {
    class InputManager;
}

namespace XE::Graphics {
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
    
    class Subset;
    class Texture2D;
    class Texture3D;
    class Texture2DArray;
    class TextureCubeMap;
    class Program;
    class Material;
    
    struct ProgramSource {
        ShaderType type;
        std::string text;
    };

    struct ProgramDescriptor {
        std::vector<ProgramSource> sources;
    };

    /**
     * @brief Describes a Vertex Attribute for use in the vertex shader
     */
    struct VertexAttribute {
        //! Attribute Name in the Vertex Shader
        std::string name;

        //! Basic data type
        DataType type;

        //! Attribute dimension (1, 2, 3 or 4)
        int size;
    };

    /**
     * @brief Describes a geometry subset
     */
    struct SubsetDescriptor {
        //! The vertex structure
        std::vector<VertexAttribute> attributes;

        //! The buffers that have data
        std::unique_ptr<Buffer> buffers;

        //! The mapping between the buffers and the vertex attributes
        std::map<std::string, int> bufferMapping;
    };

    /**
     * @brief Graphics API abstraction
     */
    class GraphicsDevice {
    public:
        virtual ~GraphicsDevice();

        virtual XE::Input::InputManager* GetInputManager() = 0;

        virtual std::unique_ptr<Subset> CreateSubset(const SubsetDescriptor& desc) = 0;
        
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferDescriptor &bufferDescriptor) = 0;
        
        virtual std::unique_ptr<Texture2D> CreateTexture2D(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture3D> CreateTexture3D(const PixelFormat format, const XE::Math::Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) = 0;
        
        virtual std::unique_ptr<Texture2DArray> CreateTexture2DArray(const PixelFormat format, const XE::Math::Vector2i &size, const int count, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) = 0;
        
        virtual std::unique_ptr<TextureCubeMap> CreateTextureCubeMap(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) = 0;
        
        virtual std::unique_ptr<Program> CreateProgram(const ProgramDescriptor &programDescriptor) = 0;
        
        virtual void SetMaterial(const Material *material) = 0;
        
        virtual const Material* GetMaterial() const = 0;

        virtual void Draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) = 0;
        
        virtual void BeginFrame(const ClearFlags flags, const XE::Math::Vector4f &color, const float depth, const int stencil) = 0;
        
        virtual void EndFrame() = 0;
    };
}

#endif
