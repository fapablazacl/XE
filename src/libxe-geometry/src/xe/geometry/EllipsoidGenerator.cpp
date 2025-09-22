
#include "xe/math/Vector.h"
#include <vector>
#include <xe/geometry/EllipsoidGenerator.h>

namespace XE {
    EllipsoidGenerator::EllipsoidGenerator(const int slices, const int stacks, const Vector3 &dimensions) : slices(slices), dimensions(dimensions), stacks(stacks) {
        
        
        
    }

    EllipsoidGenerator::~EllipsoidGenerator() {
    }

    std::vector<Vector3> EllipsoidGenerator::generateVertexCoordinates() const {
        return {};
    }

    std::vector<Vector3> EllipsoidGenerator::generateVertexNormals() const {
        return {};
    }

    std::vector<Vector2> EllipsoidGenerator::generateVertexTexCoords() const {
        return {};
    }

    std::vector<int> EllipsoidGenerator::generateIndices() const {
        return {};
    }
} // namespace XE
