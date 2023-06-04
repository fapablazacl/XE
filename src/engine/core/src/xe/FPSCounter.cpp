//
// Created by Felipe Apablaza on 26-03-23.
//

#include "FPSCounter.h"

bool FPSCounter::frame() {
    const int milliseconds = XE::Timer::getTick() - lastTime;

    if (milliseconds < 1000) {
        currentFrames++;

        return false;
    } else {
        fps = currentFrames;
        currentFrames = 0;
        lastTime = XE::Timer::getTick();

        return true;
    }
}
