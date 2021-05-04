
#include <XE/Graphics/Texture2DArray.hpp>

namespace XE {
	Texture2DArray::~Texture2DArray() {}
    
	TextureType Texture2DArray::getType() const {
		return TextureType::Tex2DArray;
	}
}
