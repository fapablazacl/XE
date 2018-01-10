
#ifndef __XE_GRAPHICS_TEXTURE_HPP__
#define __XE_GRAPHICS_TEXTURE_HPP__

#include <XE/Math/Vector.hpp>
#include <XE/Graphics/PixelFormat.hpp>

namespace XE::Graphics {
    enum class TextureType {
        Unknown, 
        Tex1D,
        Tex2D,
        Tex3D,
        TexCubeMap, 
        Tex2DArray
    };

    enum class TextureCubeMapSide {
        Unknown, 
        PositiveX, NegativeX, 
        PositiveY, NegativeY, 
        PositiveZ, NegativeZ
    };

    struct TextureDesc {
        PixelFormat Format;
        TextureType Type;
        XE::Math::Vector3f Size;
    };

    class Texture {
    public:
        virtual ~Texture();

        virtual TextureDesc GetDesc() const = 0;

        // virtual void SetData(const void *pixels, const ImageFormat::Enum format, const glm::ivec2 &offset, const glm::ivec2& size) = 0;
        // virtual void SetData(const void *pixels, const int arrayIndex, const ImageFormat::Enum format, const glm::ivec2 &offset, const glm::ivec2& size) = 0;
    };
}

#endif
