
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <string>
#include <vector>
#include <xe/graphics/Subset.h>
#include <xe/math/Vector.h>

#include "Common.h"

namespace Sandbox {
    const inline std::string ASSET_MODEL_SIMPLE_CUBE = "ASSET_MODEL_SIMPLE_CUBE";
    const inline std::string ASSET_SHADER_MAIN_VERT = "SHADER_MAIN_VERT";
    const inline std::string ASSET_SHADER_MAIN_FRAG = "SHADER_MAIN_FRAG";

    struct Assets {
        static MeshPrimitive getSquareMeshPrimitive();
    };
} // namespace Sandbox

#endif
