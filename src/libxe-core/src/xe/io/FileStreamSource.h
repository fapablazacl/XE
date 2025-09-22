
#ifndef __XE_IO_FILESTREAMSOURCE_HPP__
#define XE_IO_FILESTREAMSOURCE_HPP_

#include <memory>
#include <string>

#include "StreamSource.h"

namespace XE {
    class Stream;
    class FileStreamSource : public StreamSource {
    public:
        FileStreamSource(const std::string &directory);

        std::unique_ptr<Stream> open(const std::string &path) override;

        bool exists(const std::string &path) const override;

    private:
        std::string directory;
    };
} // namespace XE

#endif
