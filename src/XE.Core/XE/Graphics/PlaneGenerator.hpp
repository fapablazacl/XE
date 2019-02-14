
#ifndef __XE_GRAPHICS_PLANEGENERATOR_HPP__
#define __XE_GRAPHICS_PLANEGENERATOR_HPP__

#include <vector>
#include <XE/Math.hpp>

#include "ShapeGenerator.hpp"

namespace XE {
    /**
     * @brief Generate a geometry mesh over the plane XZ
     */

    // TODO: Add the following generation parameters: Reference Plane, Rotation
    // TODO: Add a method to return required rendering parameters via the SubsetEnvelope class
    // TODO: Add control parameter for the Face Vertex Ordering
    class PlaneGenerator : public ShapeGenerator {
    public:
        PlaneGenerator(const int widthParts, const int depthParts, const float width, const float depth);

        virtual ~PlaneGenerator();

        virtual std::vector<Vector3f> GenerateVertexCoordinates() const override;

        virtual std::vector<Vector3f> GenerateVertexNormals() const override;

        virtual std::vector<Vector2f> GenerateVertexTexCoords() const override;

        virtual std::vector<int> GenerateIndices() const override;

    private:
        int widthParts;
        int depthParts;

        float width;
        float depth;
    };
}

#endif
