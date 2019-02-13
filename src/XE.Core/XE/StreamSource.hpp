
#ifndef __XE_STREAMSOURCE_HPP__
#define __XE_STREAMSOURCE_HPP__

#include <memory>
#include <string>

namespace XE {
    class Stream;
    class StreamSource {
    public:
        virtual ~StreamSource();

        virtual std::unique_ptr<Stream> open(const std::string &path) = 0;

    public:
        static std::unique_ptr<StreamSource> create();
        static std::unique_ptr<StreamSource> create(const std::string &directory);
    };
}

#endif
