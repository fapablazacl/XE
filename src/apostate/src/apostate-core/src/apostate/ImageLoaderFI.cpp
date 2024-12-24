
#include "ImageLoaderFI.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>

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

static std::map<std::string, FREE_IMAGE_FORMAT> typeFIOMap = {
        {".png", FIF_PNG},
        {".jpg", FIF_JPEG},
        {".jpeg", FIF_JPEG}
};

static FREE_IMAGE_FORMAT mapType(const std::string &type) {
    const auto it = typeFIOMap.find(type);
    if (it == typeFIOMap.end()) {
        return FIF_UNKNOWN;
    }

    return it->second;
}


ImageLoaderFI::ImageLoaderFI() {
    FreeImage_Initialise();
}

ImageLoaderFI::~ImageLoaderFI() {
    FreeImage_DeInitialise();
}

std::unique_ptr<Image> ImageLoaderFI::loadImage(const std::string &file) const {
    std::filesystem::path path{file};

    if (!std::filesystem::exists(path)) {
        std::cerr << "File: \"" << file << "\" doesn't exists." << std::endl;
        return {};
    }

    const std::string type = path.extension().string();
    FREE_IMAGE_FORMAT imageType = mapType(type);  // Assuming PNG for simplicity
    FIBITMAP *bitmap = FreeImage_Load(imageType, file.c_str());
    if (!bitmap) {
        std::cerr << "Image load failed: \"" << file << "\"" << std::endl;
        return {};
    }

    FIBITMAP *convertedBitmap = FreeImage_ConvertTo24Bits(bitmap);

    FreeImage_Unload(bitmap);

    return std::make_unique<ImageFI>(convertedBitmap);
}

std::unique_ptr<Image> ImageLoaderFI::loadImage(const void *data, const size_t size, const std::string &compressionFormat) const {
    FREE_IMAGE_FORMAT imageType = mapType(compressionFormat);

    FIMEMORY *mem = FreeImage_OpenMemory((BYTE *)data, static_cast<DWORD>(size));
    FIBITMAP *bitmap = FreeImage_LoadFromMemory(imageType, mem);

    if (!bitmap) {
        std::cerr << "Image load failed: \"" << compressionFormat << "\"" << std::endl;
        FreeImage_CloseMemory(mem);
        return {};
    }

    FIBITMAP *convertedBitmap = FreeImage_ConvertTo24Bits(bitmap);

    FreeImage_Unload(bitmap);
    FreeImage_CloseMemory(mem);

    return std::make_unique<ImageFI>(convertedBitmap);
}
