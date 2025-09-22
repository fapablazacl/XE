
#ifndef __XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP__
#define XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP_

#include <vector>
#include <xe/math/Math.h>

#include "ShapeGenerator.h"

namespace XE {
    class EllipsoidGenerator : public ShapeGenerator {
    public:
        EllipsoidGenerator(int slices, int stacks, const Vector3 &dimensions);

        ~EllipsoidGenerator() override;

        std::vector<Vector3> generateVertexCoordinates() const override;

        std::vector<Vector3> generateVertexNormals() const override;

        std::vector<Vector2> generateVertexTexCoords() const override;

        std::vector<int> generateIndices() const override;

    private:
        int slices = 0;
        int stacks = 0;

        Vector3 dimensions;
    };
} // namespace XE

#endif
