
#ifndef __XE_GRAPHICS_TEXTURE_HPP__
#define __XE_GRAPHICS_TEXTURE_HPP__

#include <XE/Math/Vector.hpp>
#include <XE/Math/Rect.hpp>
#include <XE/Math/Box.hpp>
#include <XE/Graphics/PixelFormat.hpp>

namespace XE::Graphics {
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
    
        virtual PixelFormat GetFormat() const = 0;
        
        virtual TextureType GetType() const = 0;
    };
    
    class Texture2D : public Texture {
    public:
        virtual ~Texture2D() {}
        
        virtual XE:: Math::Vector2i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Recti &area) const = 0;
    };
    
    class Texture2DArray : public Texture {
    public:
        virtual ~Texture2DArray() {}
        
        virtual XE:: Math::Vector2i GetSize() const = 0;
        
        virtual int GetCount() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const XE::Math::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat     surfaceFormat, const XE::Math::Recti &area) const = 0;
    };
    
    class Texture3D : public Texture {
    public:
        virtual ~Texture3D() {}
        
        virtual XE:: Math::Vector3i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Boxi &volume) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const XE::Math::Boxi &volume) const = 0;
    };
    
    class TextureCubeMap : public Texture {
    public:
        virtual ~TextureCubeMap() {}
        
        virtual XE:: Math::Vector2i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const XE::Math::Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const XE::Math::Recti &area) const = 0;
    };
}

#endif
