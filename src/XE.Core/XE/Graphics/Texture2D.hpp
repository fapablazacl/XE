
#ifndef __XE_GRAPHICS_TEXTURE2D_HPP__
#define __XE_GRAPHICS_TEXTURE2D_HPP__

#include <cstdint>
#include <cstddef>
#include <XE/Predef.hpp>
#include <XE/Graphics/Texture.hpp>

namespace XE {
    enum class DataType : std::int16_t;

    class Texture2D : public Texture {
    public:
        virtual TextureType GetType() const override;

    public:
        virtual ~Texture2D();
        
        virtual Vector2i GetSize() const = 0;
        
        virtual void SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Recti &area) = 0;

        virtual void GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const = 0;
    };
}

#endif
