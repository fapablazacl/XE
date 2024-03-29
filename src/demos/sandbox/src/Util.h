
#pragma once

#include "Common.h"

namespace demo {
    XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs);

    Mesh makeIndexedCubeMesh(const float width, const float height, const float depth);

    Mesh makeAxisMesh(const float width, const float height, const float depth);

    Mesh makeColoredCubeMesh(const float width, const float height, const float depth);

    std::string loadTextFile(const std::string &filePath);

    XE::Subset *createCubeSubset2(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    XE::Subset *createSubset(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    Mesh makeGridMesh(const float tileSize, const int tilesInX, const int tilesInZ);

    Mesh makeCubeMesh(const float width, const float height, const float depth);
} // namespace demo
