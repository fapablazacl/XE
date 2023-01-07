
#ifndef __XE_IO_STREAMSOURCEIMPL_HPP__
#define __XE_IO_STREAMSOURCEIMPL_HPP__

#include "StreamSource.h"

namespace XE {
    class StreamFactory {
    public:
        virtual ~StreamFactory() = 0;
        virtual std::unique_ptr<Stream> create() = 0;
    };

    class StreamSourceImpl : public StreamSource {
    public:
        virtual ~StreamSourceImpl();

        virtual std::unique_ptr<Stream> open(const std::string &path) override;

        virtual bool exists(const std::string &path) const override;

    public:
        void registerFactory(const std::string &path, std::unique_ptr<StreamFactory> factory);

    private:
    };
} // namespace XE

#endif
