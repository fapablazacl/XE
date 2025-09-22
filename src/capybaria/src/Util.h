
#pragma once

#include "Common.h"

namespace demo {
    XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs);

    Mesh makeIndexedCubeMesh(float width, float height, float depth);

    Mesh makeAxisMesh(float width, float height, float depth);

    Mesh makeColoredCubeMesh(float width, float height, float depth);

    std::string loadTextFile(const std::string &filePath);

    XE::Subset *createCubeSubset2(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    XE::Subset *createSubset(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    Mesh makeGridMesh(float tileSize, int tilesInX, int tilesInZ);

    Mesh makeCubeMesh(float width, float height, float depth);
} // namespace demo
