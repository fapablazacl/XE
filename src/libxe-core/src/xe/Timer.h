
#ifndef __XE_TIMER_HPP__
#define XE_TIMER_HPP_

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
