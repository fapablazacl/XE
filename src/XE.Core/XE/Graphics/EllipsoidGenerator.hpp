
#ifndef __XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP__
#define __XE_GRAPHICS_ELLIPSOIDGENERATOR_HPP__

#include <vector>
#include <XE/Math.hpp>

#include "ShapeGenerator.hpp"

namespace XE {
    class EllipsoidGenerator : public ShapeGenerator {
    public:
        EllipsoidGenerator(const int slices, const int stacks, const Vector3f &dimensions);

        virtual ~EllipsoidGenerator();

        virtual std::vector<Vector3f> GenerateVertexCoordinates() const override;

        virtual std::vector<Vector3f> GenerateVertexNormals() const override;

        virtual std::vector<Vector2f> GenerateVertexTexCoords() const override;

        virtual std::vector<int> GenerateIndices() const override;

    private:
        int slices = 0;
        int stacks = 0;

        Vector3f dimensions;
    };
}

#endif
