
#ifndef __XE_GRAPHICS_VERTEXFORMAT_HPP__
#define __XE_GRAPHICS_VERTEXFORMAT_HPP__

#include <XE/DataAttribute.hpp>

namespace XE::Graphics {
    class VertexFormat {
    public:
        virtual ~VertexFormat();

        virtual int GetAttributeCount() const = 0;

        virtual DataAttribute *GetAttribute() = 0;

        virtual const DataAttribute* GetAttribute() const = 0;
    };
}

#endif
