
#ifndef __XE_GRAPHICS_SHAPEGENERATOR_HPP__
#define __XE_GRAPHICS_SHAPEGENERATOR_HPP__

#include <vector>
#include <xe/math/Math.h>

namespace XE {
    class ShapeGenerator {
    public:
        virtual ~ShapeGenerator();

        virtual std::vector<Vector3> generateVertexCoordinates() const = 0;

        virtual std::vector<Vector3> generateVertexNormals() const = 0;

        virtual std::vector<Vector2> generateVertexTexCoords() const = 0;

        virtual std::vector<int> generateIndices() const = 0;
    };
} // namespace XE

#endif
