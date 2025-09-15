
#include "xe/ImageLoader.h"
#include "ImageLoaderFI.h"

std::unique_ptr<ImageLoader> createImageLoader() {
    return std::make_unique<ImageLoaderFI>();
}
