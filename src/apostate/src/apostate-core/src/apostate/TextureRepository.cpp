
#include <apostate/TextureRepository.h>

#include <iostream>
#include <cassert>
#include <apostate/Renderer.h>
#include <apostate/Image.h>
#include <apostate/ImageLoader.h>

TextureRepository::TextureRepository(ImageLoader &loader) : loader{loader} {}


GLuint TextureRepository::getOrCreate(const std::string &filePath, Renderer &renderer) {
    if (filePath.empty()) {
        std::cout << "TextureRepository::getOrCreate: " <<  "Can't load texture with empty filepath" << std::endl;
        return 0;
    }
    
    if (auto it = cachedTextureMap.find(filePath); it != cachedTextureMap.end()) {
        return it->second;
    }
    
    GLuint texture = createTexture(filePath.c_str(), renderer);
    if (! texture) {
        std::cout << "TextureRepository::getOrCreate: " <<  "Texture file couldn't be loaded: '" << filePath << "'" << std::endl;
        return 0;
    }
    
    std::cout << "TextureRepository::getOrCreate: Loaded texture '" << filePath << "'" << std::endl;
    
    cachedTextureMap[filePath] = texture;
    
    return texture;
}


GLuint TextureRepository::createTexture(const char* theFileName, Renderer &renderer) {
    assert(theFileName);
    assert(std::string(theFileName) != "");

    const std::unique_ptr<Image> image = loader.loadImage(theFileName);
    const ImageData imageData = image->getData();

    return createTexture(renderer, imageData);
}


GLuint TextureRepository::createTexture(Renderer &renderer, const std::string &imageTypeStr, const size_t size, const void *imageData) const {
    const auto image = loader.loadImage(imageData, size, imageTypeStr);
    return createTexture(renderer, image->getData());
}


GLuint TextureRepository::createTexture(Renderer &renderer, const ImageData &image) const {
    GLenum internalFormat = 0;

    switch (image.bpp) {
    case 3: internalFormat = GL_RGB; break;
    case 4: internalFormat = GL_RGBA; break;
    default: internalFormat = GL_RGB;
    }
    
    return renderer.createTexture(internalFormat, image.width, image.height, internalFormat, GL_UNSIGNED_BYTE, image.pixels);
}
