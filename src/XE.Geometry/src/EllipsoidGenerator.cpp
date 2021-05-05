
#include <XE/Geometry/EllipsoidGenerator.h>

namespace XE {
    EllipsoidGenerator::EllipsoidGenerator(const int slices, const int stacks, const Vector3f &dimensions) {
        this->slices = slices;
        this->stacks = stacks;
        this->dimensions = dimensions;
    }

    EllipsoidGenerator::~EllipsoidGenerator() {}

    std::vector<Vector3f> EllipsoidGenerator::generateVertexCoordinates() const {
        return {};
    }

    std::vector<Vector3f> EllipsoidGenerator::generateVertexNormals() const {
        return {};
    }

    std::vector<Vector2f> EllipsoidGenerator::generateVertexTexCoords() const {
        return {};
    }

    std::vector<int> EllipsoidGenerator::generateIndices() const {
        return {};
    }
}
