
#ifndef __XE_GRAPHICS_SUBSET_HPP__
#define __XE_GRAPHICS_SUBSET_HPP__

#include <string>
#include <XE/Buffer.hpp>

namespace XE::Graphics {
    
    struct SubsetAttrib {
        
    };
    
    class Subset {
    public:
        virtual ~Subset() {}
        virtual int GetBufferCount() const = 0;
        virtual Buffer* GetBuffer(const int index) = 0;
        virtual Buffer* GetIndexBuffer() = 0;
        
        virtual const Buffer* GetBuffer(const int index) const = 0;
        virtual const Buffer* GetIndexBuffer() const = 0;
    };
}

#endif
