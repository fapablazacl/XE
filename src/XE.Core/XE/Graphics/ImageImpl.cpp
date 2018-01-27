
#include "ImageImpl.hpp"

#include <cstdlib>
#include <cassert>

namespace XE::Graphics {
    ImageImpl::ImageImpl() {}

    ImageImpl::~ImageImpl() {
        this->Free();
    }

    std::byte* ImageImpl::GetPointer() const {
        return pixels;
    }

    PixelFormat ImageImpl::GetFormat() const {
        return format;
    }

    XE::Math::Vector2i ImageImpl::GetSize() const {
        return size;
    }

    void ImageImpl::Allocate(const PixelFormat format, const XE::Math::Vector2i &size) {
        this->Free();

        const int pixelByteSize = ComputeStorage(format);
        const int byteSize = size.X * size.Y * pixelByteSize;

        assert(byteSize);

        pixels = static_cast<std::byte*>(std::malloc(byteSize));
    }

    void ImageImpl::Free() {
        if (pixels) {
            std::free(pixels);
            pixels = nullptr;

            format = PixelFormat::Unknown;
            size = {0, 0};
        }
    }
}
