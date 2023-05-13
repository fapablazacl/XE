
#ifndef __XE_TIMER_HPP__
#define __XE_TIMER_HPP__

#include <cstdint>
#include <xe/Predef.h>

namespace XE {
    class XE_API Timer {
    public:
        Timer() = delete;

        static uint32_t getTick();
    };
} // namespace XE

#endif
