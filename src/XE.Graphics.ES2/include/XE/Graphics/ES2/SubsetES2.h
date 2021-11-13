
#ifndef __XE_GRAPHICS_ES_SUBSETS_H__
#define __XE_GRAPHICS_ES_SUBSETS_H__

#include <XE/Graphics/Subset.h>
#include <glad/glad.h>

#include "BufferES2.h"

namespace XE {
    class SubsetES : public Subset {
    public:
        explicit SubsetES(const SubsetDescriptor2& desc);

        virtual ~SubsetES();
        
        virtual int getBufferCount() const override;

        virtual BufferES2* getBuffer(const int index) override;

        virtual BufferES2* getIndexBuffer() override;

        virtual const BufferES2* getBuffer(const int index) const override;

        virtual const BufferES2* getIndexBuffer() const override;

    public:
        GLuint GetID() const {
            return m_id;
        }

    private:
        GLuint m_id;
        std::vector<std::unique_ptr<BufferES2>> m_buffers;
        std::unique_ptr<BufferES2> m_indexBuffer;

        SubsetDescriptor2 m_descriptor;
    };
}

#endif
