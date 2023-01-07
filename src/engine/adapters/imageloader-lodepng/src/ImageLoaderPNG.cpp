
#include <xe/graphics/png/ImageLoaderPNG.h>

#include <iostream>
#include <lodepng.h>
#include <vector>
#include <xe/graphics/Image.h>
#include <xe/io/Stream.h>

namespace XE {
    class ImagePNG : public Image {
    public:
        ImagePNG(const void *pointer, const PixelFormat format, const Vector2i size) {
            this->pointer = pointer;
            this->format = format;
            this->size = size;
        }

        virtual ~ImagePNG() {}

        virtual const void *getPointer() const override { return pointer; }

        virtual PixelFormat getFormat() const override { return format; }

        virtual Vector2i getSize() const override { return size; }

    private:
        const void *pointer;
        PixelFormat format;
        Vector2i size;
    };

    ImageLoaderPNG::~ImageLoaderPNG() {}

    std::unique_ptr<Image> ImageLoaderPNG::load(Stream *inputStream) {
        // TODO: Add support for another pixel formats
        // TODO: Enhance stream buffering code

        const std::uint32_t bufferLength = 512;

        std::uint32_t readed;
        std::uint8_t buffer[bufferLength];

        std::vector<std::uint8_t> imageBuffer;

        while ((readed = inputStream->read(buffer, 1, bufferLength)) > 0) {
            for (int i = 0; i < int(readed); i++) {
                imageBuffer.push_back(buffer[i]);
            }
        }

        unsigned char *pixels = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;

        LodePNGState state = {};

        std::cout << "[INFO] ImageLoaderPNG::load: PNG File has " << imageBuffer.size() << " byte(s)." << std::endl;

        unsigned int error = lodepng_decode(&pixels, &width, &height, &state, imageBuffer.data(), imageBuffer.size());

        if (error) {
            std::cout << "ImageLoaderPNG::load: Error at loading texture from Stream (error:" << lodepng_error_text(error) << ")" << std::endl;
            assert(false);
            return {};
        }

        return std::make_unique<ImagePNG>((const void *)pixels, PixelFormat::R8G8B8A8, Vector2i(int(width), int(height)));
    }
} // namespace XE
