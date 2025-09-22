
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define XE_GRAPHICS_GL_SUBSETGL_HPP_

#include <xe/graphics/Subset.h>

#include "BufferGL.h"
#include "gl.h"

namespace XE {
    class SubsetGL : public Subset {
    public:
        explicit SubsetGL(const SubsetDescriptor &desc);

        ~SubsetGL() override;

        int getBufferCount() const override;

        BufferGL *getBuffer(int index) override;

        BufferGL *getIndexBuffer() override;

        const BufferGL *getBuffer(int index) const override;

        const BufferGL *getIndexBuffer() const override;

    
        GLuint getID() const {
            return id;
        }

    private:
        GLuint id = 0;
        std::vector<const BufferGL *> buffers;
        const BufferGL *indexBuffer = nullptr;
        SubsetDescriptor descriptor;
    };
} // namespace XE

#endif
