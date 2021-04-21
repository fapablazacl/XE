
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <XE/Graphics/Subset.hpp>
#include <XE/Math/Vector.hpp>
#include <string>
#include <vector>

#include "Asset_CGLTF.h"

namespace XE::Sandbox {

    struct Assets {
        Assets() = delete;
        
        static std::vector<Vector3f> coordData;
        static std::vector<Vector4f> colorData;
        static std::vector<Vector3f> normalData;
        static std::vector<Vector2f> texCoordData;
        static std::vector<int> indexData;
        
        static void loadModel();
        
        static std::vector<Mesh> loadModel(const std::string &assetFilePath);
        
        static MeshPrimitive getSquareMeshPrimitive();
    };
}

#endif
