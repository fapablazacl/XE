
#ifndef __XE_GRAPHICS_BOXGENERATOR_HPP__
#define __XE_GRAPHICS_BOXGENERATOR_HPP__

#include <vector>
#include <xe/math/Math.h>

#include "ShapeGenerator.h"

namespace XE {
    /**
     * @brief Generate a geometry mesh that forms a Box-Like shape
     */
    class BoxGenerator : public ShapeGenerator {
    public:
        BoxGenerator(const Vector3i &division, const Vector3 &size);

        virtual ~BoxGenerator();

        virtual std::vector<Vector3> generateVertexCoordinates() const override;

        virtual std::vector<Vector3> generateVertexNormals() const override;

        virtual std::vector<Vector2> generateVertexTexCoords() const override;

        virtual std::vector<int> generateIndices() const override;

    private:
        std::vector<Vector3> generateBaseCoords() const;

        std::vector<Vector3> generateBaseNormals() const;

        std::vector<Vector2> generateBaseTexCoords() const;

    private:
        Vector3i division;
        Vector3 size;
    };
} // namespace XE

#endif
