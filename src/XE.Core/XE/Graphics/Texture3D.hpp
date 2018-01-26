
#ifndef __XE_GRAPHICS_TEXTURE3D_HPP__
#define __XE_GRAPHICS_TEXTURE3D_HPP__

#include <XE/Graphics/Texture.hpp>

namespace XE::Graphics {
    class Texture3D : public Texture {
    public:
        virtual ~Texture3D();
        
        virtual XE:: Math::Vector3i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Boxi &volume) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Boxi &volume) const = 0;
    };    
}

#endif
