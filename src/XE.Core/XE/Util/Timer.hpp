
#ifndef __XE_TIMER_HPP__
#define __XE_TIMER_HPP__

#include <XE/Predef.hpp>

namespace XE::Util {
    class XE_API Timer {
    public:
        Timer() = delete;

        static int GetTick();
    };
}

#endif
