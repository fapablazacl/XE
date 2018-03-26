
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <XE/Graphics/Subset.hpp>
#include <glad/glad.h>

#include "BufferGL.hpp"

namespace XE::Graphics::GL {
    class SubsetGL : public Subset {
    public:
        SubsetGL(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer);

        virtual ~SubsetGL();
        
        virtual int GetBufferCount() const override;

        virtual BufferGL* GetBuffer(const int index) override;

        virtual BufferGL* GetIndexBuffer() override;

        virtual const BufferGL* GetBuffer(const int index) const override;

        virtual const BufferGL* GetIndexBuffer() const override;

        virtual SubsetDescriptor GetDescriptor() const override {
            return m_descriptor;
        }

    public:
        GLuint GetID() const {
            return m_id;
        }

    private:
        GLuint m_id;
        std::vector<std::unique_ptr<BufferGL>> m_buffers;
        std::unique_ptr<BufferGL> m_indexBuffer;

        SubsetDescriptor m_descriptor;
    };
}

#endif
