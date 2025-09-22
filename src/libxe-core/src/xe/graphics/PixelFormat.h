
#ifndef __XE_GRAPHICS_PIXELFORMAT_HPP__
#define XE_GRAPHICS_PIXELFORMAT_HPP_

#include <tuple>

namespace XE {
    enum class PixelFormat { MetaFirst = 0, Unknown = MetaFirst, R8G8B8 = 1, R8G8B8A8 = 2, MetaCount = 3 };

    inline std::tuple<int, int, int, int> get_color_bit_count(const PixelFormat format) {
        switch (format) {
        case PixelFormat::R8G8B8:
            return {8, 8, 8, 0};
        case PixelFormat::R8G8B8A8:
            return {8, 8, 8, 8};
        default:
            return {0, 0, 0, 0};
        }
    }

    enum class DepthFormat { D24, D32 };

    inline int get_depth_bit_count(const DepthFormat format) {
        switch (format) {
        case DepthFormat::D24:
            return 24;
        case DepthFormat::D32:
            return 32;
        default:
            return 0;
        }
    }

    enum class StencilFormat { SUnused, S8 };

    extern int ComputeStorage(PixelFormat format);

    inline int get_stencil_bit_count(const StencilFormat format) {
        switch (format) {
        case StencilFormat::SUnused:
            return 0;
        case StencilFormat::S8:
            return 8;
        default:
            return 0;
        }
    }
} // namespace XE

#endif
