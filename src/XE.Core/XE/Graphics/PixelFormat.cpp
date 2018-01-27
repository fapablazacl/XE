
#include "PixelFormat.hpp"

namespace XE::Graphics {
    static int g_pixelFormatStorage[] = {
        -1, 3, 4
    };

    int ComputeStorage(const PixelFormat format) {
        return g_pixelFormatStorage[static_cast<int>(format)];
    }
}
