
#ifndef __XE_GRAPHICS_TEXTURE3D_HPP__
#define __XE_GRAPHICS_TEXTURE3D_HPP__

#include <XE/Predef.hpp>
#include <XE/Graphics/Texture.hpp>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    class Texture3D : public Texture {
    public:
        virtual TextureType GetType() const override;

    public:
        virtual ~Texture3D();
        
        virtual Vector3i GetSize() const = 0;
        
        virtual void SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) = 0;
        
        virtual void GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) const = 0;
    };    
}

#endif
