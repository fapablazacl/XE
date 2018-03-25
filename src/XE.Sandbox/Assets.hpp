
#ifndef __XE_SANDBOX_ASSETS_HPP__
#define __XE_SANDBOX_ASSETS_HPP__

#include <XE/Math/Vector.hpp>
#include <string>
#include <vector>

namespace XE::Graphics {
    struct Vertex {
        XE::Math::Vector3f coord;
        XE::Math::Vector4f color;
    };
}

namespace XE::Sandbox {
    struct Assets {
        Assets() = delete;

        static std::string vertexShader;
        static std::string fragmentShader;

        static std::vector<XE::Graphics::Vertex> triangleVertices;
        static std::vector<int> triangleIndices;
    };
}

#endif
