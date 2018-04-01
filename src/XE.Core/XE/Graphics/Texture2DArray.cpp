
#include "Texture2DArray.hpp"

namespace XE::Graphics {
	Texture2DArray::~Texture2DArray() {}
    
	TextureType Texture2DArray::GetType() const {
		return TextureType::Tex2DArray;
	}
}
