
#ifndef __XE_GRAPHICS_PNG_IMAGELOADERPNG_HPP__
#define __XE_GRAPHICS_PNG_IMAGELOADERPNG_HPP__

#include <XE/Graphics/ImageLoader.hpp>

namespace XE {
    class ImageLoaderPNG : public ImageLoader {
    public:
        virtual ~ImageLoaderPNG();

        virtual std::unique_ptr<Image> load(Stream *inputStream) override;
    };
}

#endif
