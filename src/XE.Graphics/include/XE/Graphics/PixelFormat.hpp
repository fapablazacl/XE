
#ifndef __XE_GRAPHICS_PIXELFORMAT_HPP__
#define __XE_GRAPHICS_PIXELFORMAT_HPP__

namespace XE {
    enum class PixelFormat {
        Unknown,
        R8G8B8,
        R8G8B8A8,
    };

    extern int ComputeStorage(const PixelFormat format);
}

#endif
