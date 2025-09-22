
#ifndef __XE_GRAPHICS_GRAPHICSDEVICE_HPP__
#define XE_GRAPHICS_GRAPHICSDEVICE_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <xe/DataType.h>
#include <xe/Predef.h>
#include <xe/math/Vector.h>

#include "PixelFormat.h"

namespace XE {
    class Buffer;

    /**
     * @brief Specify wich buffer will be used to clear
     */
    enum class ClearFlags { Color = 0x01, Depth = 0x02, Stencil = 0x04, ColorDepth = Color | Depth, All = Color | Depth | Stencil };

    inline bool operator&(const ClearFlags flags, const ClearFlags value) {
        const int nflags = static_cast<int>(flags);
        const int nvalue = static_cast<int>(value);

        return (nflags & nvalue) != 0;
    }

    enum class PixelFormat;
    enum class ShaderType;

    struct Viewport;
    struct BufferDescriptor;
    struct SubsetEnvelope;
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
     * @brief Graphics API abstraction
     */
    class XE_API GraphicsDevice {
    public:
        virtual ~GraphicsDevice();

        virtual Subset *createSubset(const SubsetDescriptor &desc) = 0;

        virtual Buffer *createBuffer(const BufferDescriptor &bufferDescriptor) = 0;

        virtual Texture2D *
        createTexture2D(PixelFormat format, const Vector2i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData) = 0;

        virtual Texture3D *
        createTexture3D(PixelFormat format, const Vector3i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData) = 0;

        virtual Texture2DArray *createTexture2DArray(PixelFormat format, const Vector2i &size, int count) = 0;

        virtual TextureCubeMap *
        createTextureCubeMap(PixelFormat format, const Vector2i &size, PixelFormat sourceFormat, DataType sourceDataType, const void **sourceData) = 0;

        virtual Program *createProgram(const ProgramDescriptor &programDescriptor) = 0;

        virtual void setViewport(const Viewport &viewport) = 0;

        virtual Viewport getViewport() const = 0;

        virtual void setMaterial(const Material *material) = 0;

        virtual const Material *getMaterial() const = 0;

        virtual void setProgram(const Program *program) = 0;

        virtual const Program *getProgram() const = 0;

        virtual void applyUniform(const UniformMatrix *uniformMatrix, std::size_t count, const void *data) = 0;

        virtual void applyUniform(const Uniform *uniform, std::size_t count, const void *data) = 0;

        virtual void draw(const Subset *subset, const SubsetEnvelope *envelopes, std::size_t envelopeCount) = 0;

        void beginFrame(const ClearFlags flags, const Vector4 &color) {
            beginFrame(flags, color, 1.0F, 0);
        }

        virtual void beginFrame(ClearFlags flags, const Vector4 &color, float depth, int stencil) = 0;

        virtual void endFrame() = 0;
    };
} // namespace XE

#endif
