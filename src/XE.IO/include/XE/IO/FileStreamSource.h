
#ifndef __XE_IO_FILESTREAMSOURCE_HPP__
#define __XE_IO_FILESTREAMSOURCE_HPP__

#include <string>
#include <memory>

#include "StreamSource.h"

namespace XE {
    class Stream;
    class FileStreamSource : public StreamSource {
    public:
        FileStreamSource(const std::string &directory);

        virtual std::unique_ptr<Stream> open(const std::string &path) override;

        virtual bool exists(const std::string &path) const override;

    private:
        std::string directory;
    };
}

#endif
