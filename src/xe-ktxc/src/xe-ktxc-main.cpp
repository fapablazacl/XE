
#include <string>
#include <cassert>
#include <span>
#include <filesystem>
#include <map>

#include <ktx.h>
#include <glad/glad.h>
#include <vulkan/vulkan.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include "xe/Logger.h"

using ImageSpan = std::span<uint8_t>;
using FileSpan = std::span<uint8_t>;

struct Image {
    int width = 0;
    int height = 0;
    int bitsPerChannel = 0;
    int channels = 0;
    ImageSpan data;
};

inline std::string to_string(ILenum t) {
    switch (t) {
    case IL_PNG: return "IL_PNG";
    case IL_JPG: return "IL_JPG";
    default: return "ILenum(" + std::to_string(static_cast<int>(t)) + ")";
    }
}

static std::map<std::string, ILenum> typeILMap = {
    {".png", IL_PNG},
    {".jpg", IL_JPG},
    {".jpeg", IL_JPG},
};

ILenum makeImageType(const std::string &ext) {
    if (const auto it = typeILMap.find(ext); it != typeILMap.end()) {
        return it->second;
    }

    return IL_TYPE_UNKNOWN;
}

static void logDevILErrors(const char *ctx) {
    for (ILenum err = ilGetError(); err != IL_NO_ERROR; err = ilGetError()) {
        XE_LOG_WARNING("{}: DevIL error 0x{:X}\n", ctx, int(err));
    }
}

Image describeCurrentImage() {
    /*
    if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) {
	logDevILErrors("ilConvertImage");
	return {};
    }
    */

    Image image;
    image.width = ilGetInteger(IL_IMAGE_WIDTH);
    image.height = ilGetInteger(IL_IMAGE_HEIGHT);
    image.channels = ilGetInteger(IL_IMAGE_CHANNELS);
    image.bitsPerChannel = 8;
    image.data= ImageSpan{ilGetData(), static_cast<size_t>(ilGetInteger(IL_IMAGE_SIZE_OF_DATA))};

    return image;
}

ILuint createImage(const FileSpan &fileSpan, ILenum imageType) {
    XE_LOG_INFO("Loading {} image from memory buffer {}\n", to_string(imageType), fileSpan.data());

    const auto data = fileSpan.data();
    const auto size = fileSpan.size();

    ILuint id = 0;
    ilGenImages(1, &id);
    ilBindImage(id);

    bool ok = false;
    if (imageType != 0) {
	    ok = (ilLoadL(imageType, (const ILubyte*)data, (ILuint)size) == IL_TRUE);
    }
    else {
	    ILenum detected = ilDetermineTypeL((const ILubyte*)data, (ILuint)size);
	    if (detected != IL_TYPE_UNKNOWN)
		    ok = (ilLoadL(detected, (const ILubyte*)data, (ILuint)size) == IL_TRUE);
    }

    if (!ok) {
	    logDevILErrors("ilLoadL");
	    ilDeleteImages(1, &id);
	    return {};
    }

    return id;
}

ILuint createImage(const std::string &file) {
    std::filesystem::path path{file};
    if (!std::filesystem::exists(path)) {
        XE_LOG_WARNING("Bitmap file {} doesn't exist.\n", file);
        return {};
    }

    ILuint id = 0;
    ilGenImages(1, &id);
    ilBindImage(id);

    if (!ilLoadImage(file.c_str())) {
        logDevILErrors("ilLoadImage");
        ilDeleteImages(1, &id);
        return {};
    }

    return id;
}


void writeTextureKTX(const std::string &fileName)
{
  assert((values.dimension() == 2 || values.dimension() == 3) &&
         "Expects a 3D array");

  const auto height = static_cast<uint32_t>(values.shape()[0]);
  const auto width = static_cast<uint32_t>(values.shape()[1]);
  const auto depth = values.dimension() == 3 ? static_cast<uint32_t>(values.shape()[2]) : 1;

  // assert(depth >= 1 && depth <= 4 && "Expects an image with 1 to 4 color channels");
  assert(depth == 1 && "Only gray-scale images are supported for KTX export for now");

  // Prepare KTX texture info
  ktxTextureCreateInfo createInfo{};
  createInfo.baseWidth = width;
  createInfo.baseHeight = height;
  createInfo.baseDepth = depth;
  createInfo.numDimensions = 2;
  createInfo.numLevels = 1;
  createInfo.numLayers = 1;
  createInfo.numFaces = 1;
  createInfo.isArray = KTX_FALSE;
  createInfo.generateMipmaps = KTX_FALSE;
  createInfo.glInternalformat = GL_R8;
  createInfo.vkFormat = VK_FORMAT_R8_UNORM;

  ktxTexture2* texture = nullptr;
  KTX_error_code result = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);
  if (result != KTX_SUCCESS) {
    throw std::runtime_error("Failed to create KTX texture");
  }

  result = ktxTexture_SetImageFromMemory(
    ktxTexture(texture), 0, 0, 0, values.data(), values.size());

  if (result != KTX_SUCCESS) {
    ktxTexture_Destroy(ktxTexture(texture));
    throw std::runtime_error("Failed to upload image to KTX texture");
  }

  const bool compress = true;

  if (compress) {
    // Basis compression parameters
    ktxBasisParams params{};

    // ETC1S (smaller) or UASTC (higher quality) can be chosen
    params.structSize = sizeof(ktxBasisParams);
    params.uastc = KTX_FALSE; // set to KTX_TRUE for UASTC instead of ETC1S
    params.verbose = KTX_FALSE;

    result = ktxTexture2_CompressBasisEx(texture, &params);
    if (result != KTX_SUCCESS) {
      ktxTexture_Destroy(ktxTexture(texture));
      throw std::runtime_error("Failed to compress KTX texture with BasisU");
    }
  }

  ktxTexture_WriteToNamedFile(ktxTexture(texture), fileName.c_str());
}

int main(int argc,char *argv[]) {
    ilInit();
    iluInit();

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    ilShutDown();

    return 0;
}
