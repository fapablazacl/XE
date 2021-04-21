
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <XE/Graphics/Subset.hpp>
#include <XE/Math/Vector.hpp>
#include <string>
#include <vector>

#include "Asset_CGLTF.h"

namespace XE::Sandbox {

    struct Assets {
        static MeshPrimitive getSquareMeshPrimitive();
    };
}

#endif
