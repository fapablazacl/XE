
#include "Assets.hpp"

namespace XE::Sandbox {
    const char* Assets::vertexShader = R"(
#version 430

in vec3 vertCoord;
in vec4 vertColor;

out vec4 fragColor;

void main() {
    gl_Position = vec4(vertCoord, 1.0);
    fragColor = vertColor;
}
    )";

    const char* Assets::fragmentShader = R"(
#version 430

in vec4 fragColor;
out vec4 finalColor;

void main() {
    finalColor = fragColor;
}
)";

    std::vector<XE::Graphics::Vertex> Assets::triangleVertices = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    };

    std::vector<int> Assets::triangleIndices = {0, 2, 4};
}
