
#ifndef __XE_GRAPHICS_PNG_IMAGELOADERPNG_HPP__
#define XE_GRAPHICS_PNG_IMAGELOADERPNG_HPP_

#include <xe/graphics/ImageLoader.h>

namespace XE {
    class ImageLoaderPNG : public ImageLoader {
    public:
        ~ImageLoaderPNG() override;

        std::unique_ptr<Image> load(Stream *inputStream) override;
    };
} // namespace XE

#endif
