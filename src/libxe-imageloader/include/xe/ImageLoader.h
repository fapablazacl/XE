
#pragma once 

#include <string>

#include "Image.h"

#include <memory>

class ImageLoader {
public:
    virtual ~ImageLoader() = default;

    virtual std::unique_ptr<Image> loadImage(const std::string &file) const = 0;

    virtual std::unique_ptr<Image> loadImage(const void *data, const size_t size, const std::string &compressionFormat) const = 0;
};

std::unique_ptr<ImageLoader> createImageLoader();
