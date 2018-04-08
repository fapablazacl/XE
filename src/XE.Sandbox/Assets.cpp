
#include "Assets.hpp"

namespace XE::Sandbox {
    const char* Assets::vertexShader = R"(
#version 420

layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec4 vertColor;

out vec4 fragColor;

uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_proj;

void main() {
    const mat4 m_mvp = m_proj * m_view * m_model;

    gl_Position = m_mvp * vec4(vertCoord, 1.0);
    fragColor = vertColor;
}
    )";

    const char* Assets::fragmentShader = R"(
#version 420

in vec4 fragColor;
out vec4 finalColor;

void main() {
    finalColor = fragColor;
}
)";

    std::vector<XE::Math::Vector3f> Assets::coordData = {
        {0.0f,  0.5f, 0.0f},
        {0.5f,  -0.5f, 0.0f},
        {-0.5f,  -0.5f, 0.0f}
    };

    std::vector<XE::Math::Vector4f> Assets::colorData = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f}
    };

    std::vector<int> Assets::indexData = {0, 1, 2};
}
