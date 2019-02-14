
#include "EllipsoidGenerator.hpp"

namespace XE {
    EllipsoidGenerator::EllipsoidGenerator(const int slices, const int stacks, const Vector3f &dimensions) {
        this->slices = slices;
        this->stacks = stacks;
        this->dimensions = dimensions;
    }

    EllipsoidGenerator::~EllipsoidGenerator() {}

    std::vector<Vector3f> EllipsoidGenerator::GenerateVertexCoordinates() const {
        return {};
    }

    std::vector<Vector3f> EllipsoidGenerator::GenerateVertexNormals() const {
        return {};
    }

    std::vector<Vector2f> EllipsoidGenerator::GenerateVertexTexCoords() const {
        return {};
    }

    std::vector<int> EllipsoidGenerator::GenerateIndices() const {
        return {};
    }
}
