
#include <XE/IO/MemoryStream.hpp>

#include <cassert>
#include <cstring>

namespace XE {
    MemoryStream::MemoryStream(const std::uint8_t *data, const std::size_t size)
        : m_data(data), m_size(size) {
        
        assert(m_data);
        assert(m_size);
    }

    MemoryStream::~MemoryStream() {}
    
    int MemoryStream::read(void *bufferOut, const int size, const int count) {
        assert(m_data);
        assert(bufferOut);
        
        std::memcpy(bufferOut, &m_data[m_offset], size * count);
        m_offset += (size * count);
    
        return count;
    }
    
    int write(const void *buffer, const int size, const int count) {
        // TODO: Add implementation
        return 0;
    }

    bool MemoryStream::seek(const int offset, const StreamOffset position) {
        switch (position) {
        case StreamOffset::Current:
            m_offset += offset;
            break;
            
        case StreamOffset::Set:
            m_offset = offset;
            break;
            
        case StreamOffset::End:
            m_offset = offset - (int)m_size;
            break;
            
        default:
            assert(false);
        }
        
        return true;
    }
    
    int MemoryStream::tell() const {
        return m_offset;
    }

    StreamFlags MemoryStream::getFlags() const {
        return StreamFlags::Readable;
    }
}
