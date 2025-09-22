
#include "xe/ImageLoader.h"
#include "ImageLoaderIL.h"
#include <memory>

std::unique_ptr<ImageLoader> createImageLoader() {
    return std::make_unique<ImageLoaderIL>();
}
