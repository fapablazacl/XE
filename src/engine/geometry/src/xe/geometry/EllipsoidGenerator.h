
#ifndef __XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP__
#define __XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP__

#include <vector>
#include <xe/math/Math.h>

#include "ShapeGenerator.h"

namespace XE {
    class EllipsoidGenerator : public ShapeGenerator {
    public:
        EllipsoidGenerator(const int slices, const int stacks, const Vector3 &dimensions);

        virtual ~EllipsoidGenerator();

        virtual std::vector<Vector3> generateVertexCoordinates() const override;

        virtual std::vector<Vector3> generateVertexNormals() const override;

        virtual std::vector<Vector2> generateVertexTexCoords() const override;

        virtual std::vector<int> generateIndices() const override;

    private:
        int slices = 0;
        int stacks = 0;

        Vector3 dimensions;
    };
} // namespace XE

#endif
