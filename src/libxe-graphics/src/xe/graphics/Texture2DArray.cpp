
#include <xe/graphics/Texture2DArray.h>

namespace xe {
    Texture2DArray::~Texture2DArray() {
    }

    TextureType Texture2DArray::getType() const {
        return TextureType::Tex2DArray;
    }
} // namespace xe
