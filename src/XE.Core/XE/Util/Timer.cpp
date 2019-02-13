
#include "Timer.hpp"

#if defined(_WINDOWS)
#include <Windows.h>

namespace XE {
    int Timer::GetTick() {
        const DWORD dwTickCount = ::GetTickCount();
        return dwTickCount;
    }
}

#elif defined(__APPLE__)
#include <mach/mach_time.h>
#include <cassert>

// Returns monotonic time in nanos, measured from the first time the function
// is called in the process.
static uint64_t monotonicTimeNanos() {
    uint64_t now = mach_absolute_time();

    static struct Data {
        Data(uint64_t bias_) : bias(bias_) {
            kern_return_t mtiStatus = mach_timebase_info(&tb);
            assert(mtiStatus == KERN_SUCCESS);
        }
        uint64_t scale(uint64_t i) {
            return scaleHighPrecision(i - bias, tb.numer, tb.denom);
        }
        static uint64_t scaleHighPrecision(uint64_t i, uint32_t numer, uint32_t denom) {
            uint64_t high = (i >> 32) * numer;
            uint64_t low = (i & 0xffffffffull) * numer / denom;
            uint64_t highRem = ((high % denom) << 32) / denom;
            high /= denom;
            return (high << 32) + highRem + low;
        }

        mach_timebase_info_data_t tb;
        uint64_t bias;
    } data(now);

    return data.scale(now);
}

namespace XE::Util {
    int Timer::GetTick() {
        const uint64_t nanos = monotonicTimeNanos();
        return nanos / 1000000;
    }
}

#elif defined(__linux__)
#include <time.h>

namespace XE::Util {
    int Timer::GetTick() {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        return 1000 * ts.tv_sec + ts.tv_nsec / 1000000;
    }
}

#else
#error "Unsupported platform"
#endif
