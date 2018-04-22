
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <XE/Math/Vector.hpp>
#include <string>
#include <vector>

namespace XE::Sandbox {
    struct Assets {
        Assets() = delete;

        static const char* vertexShader;
        static const char* fragmentShader;

        static std::vector<XE::Math::Vector3f> coordData;
        static std::vector<XE::Math::Vector4f> colorData;
        static std::vector<XE::Math::Vector3f> normalData;
        static std::vector<XE::Math::Vector2f> texCoordData;
        static std::vector<int> indexData;
    };
}

#endif
