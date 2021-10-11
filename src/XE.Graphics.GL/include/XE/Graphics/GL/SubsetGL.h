
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <XE/Graphics/Subset.h>
#include <glad/glad.h>

#include "BufferGL.h"

namespace XE {
    class SubsetGL : public Subset {
    public:
        explicit SubsetGL(const SubsetDescriptor2 &desc);

        virtual ~SubsetGL();
        
        virtual int getBufferCount() const override;

        virtual BufferGL* getBuffer(const int index) override;

        virtual BufferGL* getIndexBuffer() override;

        virtual const BufferGL* getBuffer(const int index) const override;

        virtual const BufferGL* getIndexBuffer() const override;

    public:
        GLuint GetID() const {
            return m_id;
        }

    private:
        GLuint m_id = 0;
        std::vector<const BufferGL*> m_buffers;
        const BufferGL* m_indexBuffer = nullptr;
        SubsetDescriptor2 mDesc;
    };
}

#endif
