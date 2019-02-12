
#ifndef __XE_STREAMSOURCE_HPP__
#define __XE_STREAMSOURCE_HPP__

#include <memory>

namespace XE {
    class Stream;
    class StreamSource {
    public:
        virtual ~StreamSource();

        virtual std::unique_ptr<Stream> Open(const std::string &path) = 0;

    public:
        static std::unique_ptr<StreamSource> Create();
        static std::unique_ptr<StreamSource> Create(const std::string &directory);
    };
}

#endif
