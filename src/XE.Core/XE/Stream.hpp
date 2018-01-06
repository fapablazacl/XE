
#ifndef __XE_MATH_STREAM_HPP__
#define __XE_MATH_STREAM_HPP__

namespace XE {
    enum class StreamOffset {
        Set,
        Current, 
        End
    };

    enum class StreamFlags {
        Readable = 1, 
        Writtable = 2,
        Both = Readable | Writtable
    };

    class Stream {
    public:
        virtual ~Stream();

        virtual int Read(void *buffer, const int size, const int count) = 0;

        virtual int Write(const void *buffer, const int size) = 0;
        
        virtual bool Seek(const int offset, const StreamOffset position) = 0;

        virtual int Tell() const = 0;

        virtual StreamFlags GetFlags() = 0;
    };
}

#endif

