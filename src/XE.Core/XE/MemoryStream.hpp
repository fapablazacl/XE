
#ifndef __XE_MEMORYSTREAM_HPP__
#define __XE_MEMORYSTREAM_HPP__

#include <cstdint>
#include <cstddef>

#include "Stream.hpp"

namespace XE {
    class MemoryStream : public Stream {
    public:
        MemoryStream(const std::uint8_t *data, const std::size_t size);
        
        virtual ~MemoryStream();
        
        virtual int Read(void *buffer, const int size, const int count) override;

        virtual int Write(const void *buffer, const int size, const int count) override;
        
        virtual bool Seek(const int offset, const StreamOffset position) override;

        virtual int Tell() const override;

        virtual StreamFlags GetFlags() const override;
        
    private:
        const std::uint8_t *m_data;
        const std::size_t m_size;
        
        int m_offset = 0;
    };
}

#endif
