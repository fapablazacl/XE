
#ifndef __XE_GRAPHICS_SUBSET_HPP__
#define __XE_GRAPHICS_SUBSET_HPP__

namespace XE {
    class Buffer;
}

namespace XE::Graphics {
    class Subset {
    public:
        virtual ~Subset();

        virtual int GetBufferCount() const = 0;
        virtual Buffer* GetBuffer(const int index) = 0;
        virtual Buffer* GetIndexBuffer() = 0;
        
        virtual const Buffer* GetBuffer(const int index) const = 0;
        virtual const Buffer* GetIndexBuffer() const = 0;
    };
}

#endif
