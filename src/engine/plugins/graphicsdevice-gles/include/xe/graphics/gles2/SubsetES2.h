
#ifndef __XE_GRAPHICS_ES_SUBSETS_H__
#define __XE_GRAPHICS_ES_SUBSETS_H__

#include <glad/glad.h>
#include <xe/graphics/Subset.h>

#include "BufferES2.h"

namespace XE {
    class SubsetES : public Subset {
    public:
        explicit SubsetES(const SubsetDescriptor &desc);

        virtual ~SubsetES();

        virtual int getBufferCount() const override;

        virtual BufferES2 *getBuffer(const int index) override;

        virtual BufferES2 *getIndexBuffer() override;

        virtual const BufferES2 *getBuffer(const int index) const override;

        virtual const BufferES2 *getIndexBuffer() const override;

    public:
        void bind() const;

        void unbind() const;

    public:
        std::vector<const BufferES2 *> buffers = {};
        const BufferES2 *indexBuffer = nullptr;
        SubsetDescriptor descriptor = {};
    };
} // namespace XE

#endif
