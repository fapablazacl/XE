
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

struct ImageDesc {
    int width = 0;
    int height = 0;
    int bitsPerChannel = 0;
    int channels = 0;
    int bitsPerPixel = 0;
    ILenum dataType = 0;
    ILenum format = 0;
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

ImageDesc describeCurrentImage() {
    /*
    if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) {
	logDevILErrors("ilConvertImage");
	return {};
    }
    */

    ImageDesc image;
    image.width = ilGetInteger(IL_IMAGE_WIDTH);
    image.height = ilGetInteger(IL_IMAGE_HEIGHT);
    image.channels = ilGetInteger(IL_IMAGE_CHANNELS);
    image.bitsPerPixel = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
    image.dataType = ilGetInteger(IL_IMAGE_TYPE);
    image.format = ilGetInteger(IL_IMAGE_FORMAT);
    image.data = ImageSpan{ilGetData(), static_cast<size_t>(ilGetInteger(IL_IMAGE_SIZE_OF_DATA))};

    return image;
}

ILuint createImage(const FileSpan &fileSpan, ILenum imageType) {
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
    XE_LOG_INFO("Loading image: {}", file);

    std::filesystem::path path{file};
    if (!std::filesystem::exists(path)) {
        XE_LOG_ERROR("Bitmap file {} doesn't exist.\n", file);
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

const std::map<std::pair<ILuint, ILuint>, ktx_uint32_t> vkFormatMap = {
    {{IL_RGB, IL_UNSIGNED_BYTE}, VK_FORMAT_R8G8B8_UNORM},
    {{IL_RGBA, IL_UNSIGNED_BYTE}, VK_FORMAT_R8G8B8A8_UNORM},
};

ktx_uint32_t computeVkFormat(const ImageDesc &imageDesc) {
    const auto key = std::make_pair(imageDesc.format, imageDesc.dataType);

    if (const auto it = vkFormatMap.find(key); it != vkFormatMap.end()) {
        return it->second;
    }

    return VK_FORMAT_UNDEFINED;
}

void writeTextureKTX(const std::string &fileName, const ImageDesc &image)
{
    XE_LOG_INFO("Creating KTX2 texture");
  const uint32_t height = static_cast<uint32_t>(image.width);
  const uint32_t width = static_cast<uint32_t>(image.height);
  const uint32_t depth = 1;

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
  createInfo.vkFormat = computeVkFormat(image);
    assert(createInfo.vkFormat != VK_FORMAT_UNDEFINED);

  ktxTexture2* texture = nullptr;
  KTX_error_code result = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);
  if (result != KTX_SUCCESS) {
    throw std::runtime_error("Failed to create KTX texture");
  }

  result = ktxTexture_SetImageFromMemory(
    ktxTexture(texture), 0, 0, 0, image.data.data(), image.data.size());

  if (result != KTX_SUCCESS) {
    ktxTexture_Destroy(ktxTexture(texture));
    throw std::runtime_error("Failed to upload image to KTX texture");
  }

  const bool compress = true;

  if (compress) {
      XE_LOG_INFO("Compressing KTX texture");
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

    XE_LOG_INFO("Writing KTX texture to file: {}", fileName);
  ktxTexture_WriteToNamedFile(ktxTexture(texture), fileName.c_str());
}

int main(int argc,char *argv[]) {
    ilInit();
    iluInit();

    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    // const std::filesystem::path inputFilePath = argv[1];
    const std::filesystem::path inputFilePath = "/Users/fapablaza/Downloads/photos_2023_9_12_fst_brown-wood-floor.jpg";
    const std::filesystem::path outputFilePath = inputFilePath.parent_path() / "output.ktx2";

    const ILuint imageId = createImage(inputFilePath.string());
    ilBindImage(imageId);
    const ImageDesc image = describeCurrentImage();
    writeTextureKTX(outputFilePath, image);

    ilShutDown();

    return 0;
}
