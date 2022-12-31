
#ifndef __XE_GRAPHICS_TEXTURE_HPP__
#define __XE_GRAPHICS_TEXTURE_HPP__

#include <xe/math/Vector.h>
#include <xe/math/Rect.h>
#include <xe/math/Box.h>
#include <xe/graphics/PixelFormat.h>

namespace XE {
    enum class TextureType {
        Unknown,
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

    class Texture {
    public:
        virtual ~Texture();
    
        virtual PixelFormat getFormat() const = 0;
        
        virtual TextureType getType() const = 0;
    };

}

#endif
