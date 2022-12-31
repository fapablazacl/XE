
#include <xe/graphics/Texture3D.h>

namespace XE {
	Texture3D::~Texture3D() {}
    
	TextureType Texture3D::getType() const {
		return TextureType::Tex3D;
	}
}
