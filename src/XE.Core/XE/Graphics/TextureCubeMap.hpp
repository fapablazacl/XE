
#ifndef __XE_GRAPHICS_TEXTURECUBEMAP_HPP__
#define __XE_GRAPHICS_TEXTURECUBEMAP_HPP__

#include <XE/Predef.hpp>
#include <XE/Graphics/Texture.hpp>

namespace XE {
    class TextureCubeMap : public Texture {
    public:
        virtual ~TextureCubeMap();
        
        virtual XE::Vector2i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const XE::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const XE::Recti &area) const = 0;
    };
}

#endif
