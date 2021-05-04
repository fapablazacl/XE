
#include <XE/Graphics/Texture2D.hpp>

namespace XE {
	Texture2D::~Texture2D() {}

	TextureType Texture2D::getType() const {
		return TextureType::Tex2D;
	}
}
