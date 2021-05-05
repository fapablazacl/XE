
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <XE/Graphics/Subset.h>
#include <glad/glad.h>

#include "BufferGL.h"

namespace XE {
    class SubsetGL : public Subset {
    public:
        SubsetGL(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer);

        virtual ~SubsetGL();
        
        virtual int getBufferCount() const override;

        virtual BufferGL* getBuffer(const int index) override;

        virtual BufferGL* getIndexBuffer() override;

        virtual const BufferGL* getBuffer(const int index) const override;

        virtual const BufferGL* getIndexBuffer() const override;

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
