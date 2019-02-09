
#include "ImageLoaderPNG.hpp"

#include <vector>
#include <lodepng.h>
#include <XE/Stream.hpp>
#include <XE/Graphics/Image.hpp>

namespace XE::Graphics::PNG {

    class ImagePNG : public Image {
    public:
        explicit ImagePNG(const std::byte *pointer, const PixelFormat format, const XE::Math::Vector2i size) {
            this->pointer = pointer;
            this->format = format;
            this->size = size;
        }

        virtual ~ImagePNG() {}

        virtual const std::byte* GetPointer() const override {
            return pointer;
        }

        virtual PixelFormat GetFormat() const override {
            return format;
        }

        virtual XE::Math::Vector2i GetSize() const override {
            return size;
        }

    private:
        const std::byte *pointer;
        PixelFormat format;
        XE::Math::Vector2i size;
    };


    ImageLoaderPNG::~ImageLoaderPNG() {}

    std::unique_ptr<Image> ImageLoaderPNG::Load(Stream *inputStream) {
        const std::uint32_t bufferLength = 512;

        std::uint32_t readed;
        std::uint8_t buffer[bufferLength];
    
        std::vector<std::uint8_t> imageBuffer;

        while ( (readed = inputStream->Read(buffer, bufferLength, 1)) > 0) {
            for (int i=0; i<int(readed); i++) {
                imageBuffer.push_back(buffer[i]);
            }
        }

        unsigned char *pixels = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;

        LodePNGState state;

        unsigned int error = lodepng_decode(&pixels, &width, &height, &state, imageBuffer.data(), imageBuffer.size());

        return {};
    }
}
