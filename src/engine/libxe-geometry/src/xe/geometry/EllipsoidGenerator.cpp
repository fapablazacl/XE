
#include <xe/geometry/EllipsoidGenerator.h>

namespace XE {
    EllipsoidGenerator::EllipsoidGenerator(const int slices, const int stacks, const Vector3 &dimensions) {
        this->slices = slices;
        this->stacks = stacks;
        this->dimensions = dimensions;
    }

    EllipsoidGenerator::~EllipsoidGenerator() {}

    std::vector<Vector3> EllipsoidGenerator::generateVertexCoordinates() const { return {}; }

    std::vector<Vector3> EllipsoidGenerator::generateVertexNormals() const { return {}; }

    std::vector<Vector2> EllipsoidGenerator::generateVertexTexCoords() const { return {}; }

    std::vector<int> EllipsoidGenerator::generateIndices() const { return {}; }
} // namespace XE
