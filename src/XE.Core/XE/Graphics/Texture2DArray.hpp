
#ifndef __XE_GRAPHICS_TEXTURE2DARRAY_HPP__
#define __XE_GRAPHICS_TEXTURE2DARRAY_HPP__

#include <XE/Graphics/Texture.hpp>

namespace XE::Graphics {
    class Texture2DArray : public Texture {
    public:
        virtual ~Texture2DArray();
        
        virtual XE:: Math::Vector2i GetSize() const = 0;
        
        virtual int GetCount() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const XE::Math::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const XE::Math::Recti &area) const = 0;
    };
    
}

#endif
