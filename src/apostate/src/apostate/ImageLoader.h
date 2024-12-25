
#pragma once 

#include <string>

#include "Image.h"

class ImageLoader {
public:
    virtual ~ImageLoader();

    virtual std::unique_ptr<Image> loadImage(const std::string &file) const = 0;

    virtual std::unique_ptr<Image> loadImage(const void *data, const size_t size, const std::string &compressionFormat) const = 0;
};
