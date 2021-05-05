
#ifndef __XE_GRAPHICS_TEXTURECUBEMAP_HPP__
#define __XE_GRAPHICS_TEXTURECUBEMAP_HPP__

#include <XE/Predef.h>
#include <XE/Graphics/Texture.h>

namespace XE {
    class TextureCubeMap : public Texture {
    public:
        virtual ~TextureCubeMap();
        
        virtual Vector2i getSize() const = 0;
        
        virtual void setData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const Recti &area) const = 0;
    };
}

#endif
