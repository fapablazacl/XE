
#include "BoxGenerator.hpp"

#include <XE/Math/Util.hpp>

namespace XE {
    BoxGenerator::BoxGenerator(const Vector3i &division, const Vector3f &size) {
        this->division = division;
        this->size = size;
    }

    BoxGenerator::~BoxGenerator() {}

    std::vector<Vector3f> BoxGenerator::GenerateVertexCoordinates() const {
        return {};
    }

    std::vector<Vector3f> BoxGenerator::GenerateVertexNormals() const {
        return {};
    }

    std::vector<Vector2f> BoxGenerator::GenerateVertexTexCoords() const {
        return {};
    }

    std::vector<int> BoxGenerator::GenerateIndices() const {
        return {};
    }
}
