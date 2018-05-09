
#include "Timer.hpp"

#include <Windows.h>

namespace XE::Util {
    int Timer::GetTick() {
        const DWORD dwTickCount = ::GetTickCount();
        return dwTickCount;
    }
}
