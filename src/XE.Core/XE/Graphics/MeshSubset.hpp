
#ifndef __XE_GRAPHICS_MESHSUBSET_HPP__
#define __XE_GRAPHICS_MESHSUBSET_HPP__

namespace XE::Graphics {
    class Buffer;
    class MeshSubset {
    public:
        virtual ~MeshSubset();

        virtual int GetBufferCount() const = 0;
        virtual Buffer* GetBuffer(const int index) = 0;
        virtual Buffer* GetIndexBuffer() = 0;
        
        virtual const Buffer* GetBuffer(const int index) const = 0;
        virtual const Buffer* GetIndexBuffer() const = 0;
    };
}

#endif
