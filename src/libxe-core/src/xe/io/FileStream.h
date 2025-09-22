
#ifndef __XE_IO_FILESTREAM_HPP__
#define XE_IO_FILESTREAM_HPP_

#include "Stream.h"
#include <cstdio>
#include <string>

namespace XE {
    /**
     * @brief Streaming for binary data
     */
    class FileStream : public Stream {
    public:
        FileStream(const std::string &fileName, StreamFlags flags);

        ~FileStream() override;

        int read(void *buffer, int size, int count) override;

        int write(const void *buffer, int size, int count) override;

        bool seek(int offset, StreamOffset position) override;

        int tell() const override;

        StreamFlags getFlags() const override;

    private:
        std::string m_fileName;
        StreamFlags m_flags;

        FILE *m_fileHandle = nullptr;
    };
} // namespace XE

#endif
