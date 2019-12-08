
#ifndef __XE_GRAPHICS_GL_SUBSETGL_HPP__
#define __XE_GRAPHICS_GL_SUBSETGL_HPP__

#include <XE/Graphics/Subset.hpp>
#include <glad/es2/glad.h>

#include "BufferES2.hpp"

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

        virtual BufferES2* getBuffer(const int index) override;

        virtual BufferES2* getIndexBuffer() override;

        virtual const BufferES2* getBuffer(const int index) const override;

        virtual const BufferES2* getIndexBuffer() const override;

        virtual SubsetDescriptor GetDescriptor() const override {
            return m_descriptor;
        }

    public:
        GLuint GetID() const {
            return m_id;
        }

    private:
        GLuint m_id;
        std::vector<std::unique_ptr<BufferES2>> m_buffers;
        std::unique_ptr<BufferES2> m_indexBuffer;

        SubsetDescriptor m_descriptor;
    };
}

#endif
