
#include "ImageLoaderFI.h"

#include <cassert>
#include <fstream>
#include <map>
#include <filesystem>
#include "xe/Logger.h"

#include <FreeImage.h>

class ImageFI : public Image {
public:
    explicit ImageFI(FIBITMAP *bitmap) : mBitmap(bitmap) {
        assert(bitmap);
    }

    virtual ~ImageFI() {
        FreeImage_Unload(mBitmap);
    }

    ImageData getData() const override {
        ImageData data;

        data.bpp = FreeImage_GetBPP(mBitmap);
        data.width = FreeImage_GetWidth(mBitmap);
        data.height = FreeImage_GetHeight(mBitmap);
        data.pixels = FreeImage_GetBits(mBitmap);

        return data;
    }

private:
    FIBITMAP *mBitmap = nullptr;
};

inline std::string str(const FREE_IMAGE_FORMAT format) {
    switch (format) {
    case FIF_PNG: return "FIF_PNG";
    case FIF_JPEG: return "FIF_JPEG";
    case FIF_UNKNOWN: return "FIF_UNKNOWN";
    default:
        return "FREE_IMAGE_FORMAT(" + std::to_string(format) + ")";
    }
}

static std::map<std::string, FREE_IMAGE_FORMAT> typeFIOMap = {
    {".png", FIF_PNG},
    {".jpg", FIF_JPEG},
    {".jpeg", FIF_JPEG},
    {"png", FIF_PNG},
    {"jpg", FIF_JPEG},
    {"jpeg", FIF_JPEG},
};

static FREE_IMAGE_FORMAT mapType(const std::string &type) {
    const auto it = typeFIOMap.find(type);
    if (it == typeFIOMap.end()) {
        return FIF_UNKNOWN;
    }

    return it->second;
}


static FREE_IMAGE_FORMAT mapType(const ImageFormat &imageFormat) {
    switch (imageFormat) {
    case ImageFormat::Png: return FIF_PNG;
    case ImageFormat::Jpeg: return FIF_JPEG;
    default: return FIF_UNKNOWN;
    }
}

// this will get called whenever a load or save error happens
void ImageLoaderFI_OutputMessageFunction(FREE_IMAGE_FORMAT fif, const char *msg) {
    XE_LOG_WARNING("FreeImage ");

    if (fif != FIF_UNKNOWN) {
        XE_LOG_WARNING("[{}] ", FreeImage_GetFormatFromFIF(fif));
    }

    XE_LOG_WARNING("{}\n", msg);
}

ImageLoaderFI::ImageLoaderFI() {
    FreeImage_Initialise();

    FreeImage_SetOutputMessage(ImageLoaderFI_OutputMessageFunction);
}

ImageLoaderFI::~ImageLoaderFI() {
    FreeImage_DeInitialise();
}

std::unique_ptr<Image> ImageLoaderFI::loadImage(const std::string &file) const {
    std::filesystem::path path{file};

    if (!std::filesystem::exists(path)) {
        XE_LOG_WARNING("Bitmap file {} doesn't exists.\n", file);
        return {};
    }

    const std::string type = path.extension().string();
    FREE_IMAGE_FORMAT imageType = mapType(type);
    FIBITMAP *bitmap = FreeImage_Load(imageType, file.c_str());
    if (!bitmap) {
        XE_LOG_WARNING("Failed load bitmap file {} to 24 bits.\n", file);
        return {};
    }

    FIBITMAP *convertedBitmap = FreeImage_ConvertTo24Bits(bitmap);
    if (!convertedBitmap) {
        XE_LOG_WARNING("Failed to convert bitmap file {} to 24 bits.\n", file);
        FreeImage_Unload(bitmap);
        return {};
    }

    FreeImage_Unload(bitmap);

    return std::make_unique<ImageFI>(convertedBitmap);
}

std::unique_ptr<Image> createImage(const void *data, const size_t size, FREE_IMAGE_FORMAT imageType) {
    XE_LOG_INFO("Loading {} image from memory buffer {}\n", str(imageType), data);

    if (imageType == FIF_UNKNOWN) {
        XE_LOG_WARNING("Could not recognize the format {}\n", str(imageType));
        return {};
    }

    FIMEMORY *mem = FreeImage_OpenMemory((BYTE *)data, static_cast<DWORD>(size));
    FIBITMAP *bitmap = FreeImage_LoadFromMemory(imageType, mem);

    if (!bitmap) {
        XE_LOG_WARNING("Could not load the image with format {}\n", str(imageType));
        FreeImage_CloseMemory(mem);
        return {};
    }

    FIBITMAP *convertedBitmap = FreeImage_ConvertTo24Bits(bitmap);
    FreeImage_Unload(bitmap);
    FreeImage_CloseMemory(mem);

    return std::make_unique<ImageFI>(convertedBitmap);
}

std::unique_ptr<Image> ImageLoaderFI::loadImage(const void *data, const size_t size, const std::string &compressionFormat) const {
    const FREE_IMAGE_FORMAT imageType = mapType(compressionFormat);
    return createImage(data, size, imageType);
}

std::unique_ptr<Image> ImageLoaderFI::loadImage(const void *data, const size_t size, const ImageFormat &format) const {
    const FREE_IMAGE_FORMAT imageType = mapType(format);
    return createImage(data, size, imageType);
}
