
#include "ImageLoaderPNG.hpp"

#include <iostream>
#include <vector>
#include <lodepng.h>
#include <XE/Stream.hpp>
#include <XE/Graphics/Image.hpp>

namespace XE::Graphics::PNG {

    class ImagePNG : public Image {
    public:
        ImagePNG(const std::byte *pointer, const PixelFormat format, const XE::Math::Vector2i size) {
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
        // TODO: Add support for another pixel formats
        // TODO: Enhance stream buffering code

        const std::uint32_t bufferLength = 512;

        std::uint32_t readed;
        std::uint8_t buffer[bufferLength];
    
        std::vector<std::uint8_t> imageBuffer;

        while ( (readed = inputStream->Read(buffer, 1, bufferLength)) > 0) {
            for (int i=0; i<int(readed); i++) {
                imageBuffer.push_back(buffer[i]);
            }
        }

        unsigned char *pixels = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;

        LodePNGState state = {};

        std::cout << "[INFO] ImageLoaderPNG::Load: PNG File has " << imageBuffer.size() << " byte(s)."  << std::endl;

        unsigned int error = lodepng_decode(&pixels, &width, &height, &state, imageBuffer.data(), imageBuffer.size());

        if (error) {
            std::cout << "ImageLoaderPNG::Load: Error at loading texture from XE::Stream (error:" << lodepng_error_text(error) << ")" << std::endl;
            assert(false);
            return {};
        }

        return std::make_unique<ImagePNG> (
            (const std::byte*)pixels, 
            PixelFormat::R8G8B8A8, 
            XE::Math::Vector2i(int(width), int(height))
        );
    }
}
