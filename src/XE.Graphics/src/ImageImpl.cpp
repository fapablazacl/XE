
#include <XE/Graphics/ImageImpl.h>

#include <cstdlib>
#include <cassert>

namespace XE {
    ImageImpl::ImageImpl() {}

    ImageImpl::~ImageImpl() {
        this->free();
    }

    const std::byte* ImageImpl::getPointer() const {
        return pixels;
    }

    PixelFormat ImageImpl::getFormat() const {
        return format;
    }

    Vector2i ImageImpl::getSize() const {
        return size;
    }

    void ImageImpl::allocate(const PixelFormat format, const Vector2i &size) {
        this->free();

        const int pixelByteSize = ComputeStorage(format);
        const int byteSize = size.X * size.Y * pixelByteSize;

        assert(byteSize);

        pixels = static_cast<std::byte*>(std::malloc(byteSize));
    }

    void ImageImpl::free() {
        if (pixels) {
            std::free(pixels);
            pixels = nullptr;

            format = PixelFormat::Unknown;
            size = {0, 0};
        }
    }
}
