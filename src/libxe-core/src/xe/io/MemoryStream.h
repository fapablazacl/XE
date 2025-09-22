
#ifndef __XE_IO_MEMORYSTREAM_HPP__
#define XE_IO_MEMORYSTREAM_HPP_

#include <cstddef>
#include <cstdint>

#include "Stream.h"

namespace XE {
    class MemoryStream : public Stream {
    public:
        MemoryStream(std::uint8_t *data, std::size_t size);

        ~MemoryStream() override;

        int read(void *buffer, int size, int count) override;

        int write(const void *buffer, int size, int count) override;

        bool seek(int offset, StreamOffset position) override;

        int tell() const override;

        StreamFlags getFlags() const override;

    private:
        std::uint8_t *m_data = nullptr;
        const std::size_t m_size;

        int m_offset = 0;
    };
} // namespace XE

#endif
