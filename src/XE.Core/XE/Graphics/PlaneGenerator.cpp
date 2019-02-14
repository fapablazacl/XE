
#include "PlaneGenerator.hpp"

namespace XE {
    PlaneGenerator::PlaneGenerator(const Vector2i &division, const Vector2f &size) {
        this->division = division;
        this->size = size;
    }

    PlaneGenerator::~PlaneGenerator() {}

    inline float ratio(const int a, const int b) {
        return static_cast<float>(a) / static_cast<float>(b);
    }

    std::vector<Vector3f> PlaneGenerator::GenerateVertexCoordinates() const {
        std::vector<Vector3f> coordinates;

        const int slices = division.X;
        const int stacks = division.Y;

        const float left = -0.5f;
        const float right = 0.5f;

        for (int i=0; i<slices + 1; i++) {
            const float ti = ratio(i, slices);

            for (int j=0; j<stacks + 1; j++) {
                const float tj = ratio(j, stacks);
                const Vector3f coord = {
                    Lerp(-0.5f, 0.5f, tj), 
                    Lerp(0.5f, -0.5f, ti), 
                    0.0f
                };

                coordinates.push_back(coord);
            }
        }

        return coordinates;
    }

    std::vector<Vector3f> PlaneGenerator::GenerateVertexNormals() const {
        std::vector<Vector3f> normals;

        const int slices = division.X;
        const int stacks = division.Y;

        const float left = -0.5f;
        const float right = 0.5f;

        for (int i=0; i<slices + 1; i++) {
            const float ti = ratio(i, slices);

            for (int j=0; j<stacks + 1; j++) {
                const float tj = ratio(j, stacks);
                const Vector3f normal = {0.0f, 0.0f, -1.0f};

                normals.push_back(normal);
            }
        }

        return normals;
    }

    std::vector<Vector2f> PlaneGenerator::GenerateVertexTexCoords() const {
        std::vector<Vector2f> texCoords;

        const int slices = division.X;
        const int stacks = division.Y;

        const float left = -0.5f;
        const float right = 0.5f;

        for (int i = 0; i<slices + 1; i++) {
            const float ti = ratio(i, slices);

            for (int j = 0; j<stacks + 1; j++) {
                const float tj = ratio(j, stacks);
                const Vector2f texCoord = {
                    Lerp(0.0f, 1.0f, tj),
                    Lerp(1.0f, 0.0f, ti)
                };

                texCoords.push_back(texCoord);
            }
        }

        return texCoords;
    }

    std::vector<int> PlaneGenerator::GenerateIndices() const {
        const int slices = division.X;
        const int stacks = division.Y;

		std::vector<int> indices;

		for (int i=0; i<slices; i++) {
			for (int j=0; j<stacks; j++) {
				const int p0 = (i + 0) + (j + 0) * (slices + 1);
				const int p1 = (i + 1) + (j + 0) * (slices + 1);
				const int p2 = (i + 0) + (j + 1) * (slices + 1);
				const int p3 = (i + 1) + (j + 1) * (slices + 1);

				// first triangle
				indices.push_back(p0);
				indices.push_back(p1);
				indices.push_back(p2);

				// second triangle
				indices.push_back(p1);
				indices.push_back(p3);
				indices.push_back(p2);
			}
		}

        return indices;
    }
}
