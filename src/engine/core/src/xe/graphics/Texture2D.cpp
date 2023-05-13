
#include <xe/graphics/Texture2D.h>

namespace XE {
    Texture2D::~Texture2D() {}

    TextureType Texture2D::getType() const { return TextureType::Tex2D; }
} // namespace XE
