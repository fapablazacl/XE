
#ifndef __XE_TIMER_HPP__
#define __XE_TIMER_HPP__

#include <XE/Predef.h>
#include <cstdint>

namespace XE {
    class XE_API Timer {
    public:
        Timer() = delete;

        static std::uint32_t getTick();
    };
}

#endif
