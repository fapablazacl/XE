
#include "Texture2D.hpp"

namespace XE {
	Texture2D::~Texture2D() {}

	TextureType Texture2D::GetType() const {
		return TextureType::Tex2D;
	}
}
