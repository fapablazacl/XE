
#pragma once

#ifndef XE_FPSCOUNTER_H
#define XE_FPSCOUNTER_H

#include "xe/Timer.h"

class FPSCounter {
public:
    //! must be called once per frame
    bool frame();

    int getFPS() const { return fps; }

private:
    int lastTime = XE::Timer::getTick();
    int currentFrames = 0;
    int fps = 0;
};

#endif // XE_FPSCOUNTER_H
