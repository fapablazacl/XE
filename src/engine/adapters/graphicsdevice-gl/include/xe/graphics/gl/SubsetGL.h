
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <xe/graphics/Subset.h>

#include "BufferGL.h"
#include "glcore.h"

namespace XE {
    class SubsetGL : public Subset {
    public:
        explicit SubsetGL(const SubsetDescriptor &desc);

        virtual ~SubsetGL();

        virtual int getBufferCount() const override;

        virtual BufferGL *getBuffer(const int index) override;

        virtual BufferGL *getIndexBuffer() override;

        virtual const BufferGL *getBuffer(const int index) const override;

        virtual const BufferGL *getIndexBuffer() const override;

    public:
        GLuint getID() const { return id; }

    private:
        GLuint id = 0;
        std::vector<const BufferGL *> buffers;
        const BufferGL *indexBuffer = nullptr;
        SubsetDescriptor descriptor;
    };
} // namespace XE

#endif
