
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <XE/Graphics/Subset.hpp>
#include <glad/glad.h>

namespace XE::Graphics::GL {
    class BufferGL;
    class SubsetGL : public Subset {
    public:
        SubsetGL(const SubsetDescriptor &desc);

        virtual ~SubsetGL();
        
        virtual int GetBufferCount() const override;

        virtual Buffer* GetBuffer(const int index) override;

        virtual Buffer* GetIndexBuffer() override;

        virtual const Buffer* GetBuffer(const int index) const override;

        virtual const Buffer* GetIndexBuffer() const override;

    public:
        GLuint GetID() const {
            return m_id;
        }

    private:
        GLuint m_id;
        std::vector<std::unique_ptr<BufferGL>> m_buffers;
        std::unique_ptr<BufferGL> m_indexBuffer;
    };
}

#endif
