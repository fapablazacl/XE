
#ifndef __XE_GRAPHICS_SUBSET_HPP__
#define __XE_GRAPHICS_SUBSET_HPP__

#include <vector>
#include <XE/Buffer.hpp>
#include <XE/Structure.hpp>

namespace XE::Graphics {
    struct SubsetAttribute : public Attribute {
        int BufferIndex;
    };

    typedef Structure<SubsetAttribute> SubsetStructure;

    class Subset {
    public:
        virtual ~Subset() {}
        virtual const SubsetStructure* GetStructure() = 0;
        virtual int GetBufferCount() const = 0;
        virtual Buffer* GetBuffer(const int index) = 0;
        virtual Buffer* GetIndexBuffer() = 0;
        
        virtual const Buffer* GetBuffer(const int index) const = 0;
        virtual const Buffer* GetIndexBuffer() const = 0;
    };
}

#endif
