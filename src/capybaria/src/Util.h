
#pragma once

#include "Common.h"

namespace demo {
    xe::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs);

    Mesh makeIndexedCubeMesh(const float width, const float height, const float depth);

    Mesh makeAxisMesh(const float width, const float height, const float depth);

    Mesh makeColoredCubeMesh(const float width, const float height, const float depth);

    std::string loadTextFile(const std::string &filePath);

    xe::Subset *createCubeSubset2(xe::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    xe::Subset *createSubset(xe::GraphicsDevice *graphicsDevice, const Mesh &mesh);

    Mesh makeGridMesh(const float tileSize, const int tilesInX, const int tilesInZ);

    Mesh makeCubeMesh(const float width, const float height, const float depth);
} // namespace demo
