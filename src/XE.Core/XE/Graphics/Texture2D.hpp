
#ifndef __XE_GRAPHICS_TEXTURE2D_HPP__
#define __XE_GRAPHICS_TEXTURE2D_HPP__

#include <XE/Graphics/Texture.hpp>

namespace XE::Graphics {
    class Texture2D : public Texture {
    public:
        virtual TextureType GetType() const override;

    public:
        virtual ~Texture2D();
        
        virtual XE:: Math::Vector2i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Recti &area) const = 0;
    };
    
}

#endif
