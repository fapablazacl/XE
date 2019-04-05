
#include "Assets.hpp"

namespace XE::Sandbox {
    std::vector<Vector3f> Assets::coordData = {
        {-1.5f,  1.5f, 0.0f},
        { 1.5f,  1.5f, 0.0f},
        {-1.5f, -1.5f, 0.0f},
        { 1.5f, -1.5f, 0.0f}
    };

    std::vector<Vector4f> Assets::colorData = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}
    };

    std::vector<Vector3f> Assets::normalData = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}
    };

    std::vector<Vector2f> Assets::texCoordData = {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

    std::vector<int> Assets::indexData = {0, 1, 2, 3};
}
