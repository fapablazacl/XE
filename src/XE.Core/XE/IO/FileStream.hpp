
#ifndef __XE_IO_FILESTREAM_HPP__
#define __XE_IO_FILESTREAM_HPP__

#include "Stream.hpp"
#include <string>
#include <cstdio>

namespace XE {
    /**
     * @brief Streaming for binary data
     */
    class FileStream : public Stream {
    public:
        FileStream(const std::string &fileName, const StreamFlags flags);

        virtual ~FileStream();

        virtual int Read(void *buffer, const int size, const int count) override;

        virtual int Write(const void *buffer, const int size, const int count) override;
        
        virtual bool Seek(const int offset, const StreamOffset position) override;

        virtual int Tell() const override;

        virtual StreamFlags GetFlags() const override;

    private:
        std::string m_fileName;
        StreamFlags m_flags;

        FILE *m_fileHandle = nullptr;
    };
}

#endif
