
#ifndef __XE_GRAPHICS_BOXGENERATOR_HPP__
#define __XE_GRAPHICS_BOXGENERATOR_HPP__

#include <vector>
#include <XE/Math.hpp>

#include "ShapeGenerator.hpp"

namespace XE {
    /**
     * @brief Generate a geometry mesh that forms a Box-Like shape
     */
    class BoxGenerator : public ShapeGenerator {
    public:
        BoxGenerator(const Vector3i &division, const Vector3f &size);

        virtual ~BoxGenerator();

        virtual std::vector<Vector3f> GenerateVertexCoordinates() const override;

        virtual std::vector<Vector3f> GenerateVertexNormals() const override;

        virtual std::vector<Vector2f> GenerateVertexTexCoords() const override;

        virtual std::vector<int> GenerateIndices() const override;

    private:
        std::vector<Vector3f> generateBaseCoords() const;

        std::vector<Vector3f> generateBaseNormals() const;

        std::vector<Vector2f> generateBaseTexCoords() const;

    private:
        Vector3i division; 
        Vector3f size;
    };
}

#endif
