
#ifndef __XE_GRAPHICS_SHAPEGENERATOR_HPP__
#define __XE_GRAPHICS_SHAPEGENERATOR_HPP__

#include <vector>
#include <XE/Math.hpp>

namespace XE {
    class ShapeGenerator {
    public:
        virtual ~ShapeGenerator();

        virtual std::vector<Vector3f> generateVertexCoordinates() const = 0;

        virtual std::vector<Vector3f> generateVertexNormals() const = 0;

        virtual std::vector<Vector2f> generateVertexTexCoords() const = 0;

        virtual std::vector<int> generateIndices() const = 0;

    public:
    
    };
}

#endif
