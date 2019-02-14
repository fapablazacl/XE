
#include "PlaneGenerator.hpp"

namespace XE {
    PlaneGenerator::PlaneGenerator(const int widthParts, const int depthParts, const float width, const float depth) {
        this->widthParts = widthParts;
        this->depthParts = depthParts;
        this->width = width;
        this->depth = depth;
    }

    PlaneGenerator::~PlaneGenerator() {}

    std::vector<Vector3f> PlaneGenerator::GenerateVertexCoordinates() const {
        std::vector<Vector3f> coordinates;

        for (int i=0; i<widthParts; i++) {
            for (int j=0; j<depthParts; j++) {
                const Vector3f v1 = {-0.5f, 0.0f, 0.5f};
                const Vector3f v2 = {0.5f, 0.0f, 0.5f};
                const Vector3f v3 = {-0.5f, 0.0f, -0.5f};
                const Vector3f v4 = {0.5f, 0.0f, -0.5f};

                coordinates.push_back(v1); coordinates.push_back(v2); coordinates.push_back(v3);
                coordinates.push_back(v2); coordinates.push_back(v4); coordinates.push_back(v3);
            }
        }

        return coordinates;
    }

    std::vector<Vector3f> PlaneGenerator::GenerateVertexNormals() const {
        std::vector<Vector3f> normals;

        for (int i=0; i<widthParts; i++) {
            for (int j=0; j<depthParts; j++) {

                for (int k=0; k<4; k++) {
                    normals.push_back({0.0f, 1.0f, 0.0f});
                }
            }
        }

        return normals;
    }

    std::vector<Vector2f> PlaneGenerator::GenerateVertexTexCoords() const {
        std::vector<Vector2f> texCoords;

        for (int i=0; i<widthParts; i++) {
            for (int j=0; j<depthParts; j++) {
                texCoords.push_back({0.0f, 1.0f});
                texCoords.push_back({1.0f, 1.0f});
                texCoords.push_back({0.0f, 0.0f});
                texCoords.push_back({1.0f, 0.0f});
            }
        }

        return texCoords;
    }

    std::vector<int> PlaneGenerator::GenerateIndices() const {
        std::vector<int> indices;

        int pivot = 0;

        for (int i=0; i<widthParts; i++) {
            for (int j=0; j<depthParts; j++) {
                indices.push_back(pivot + 0);
                indices.push_back(pivot + 1);
                indices.push_back(pivot + 2);

                indices.push_back(pivot + 2);
                indices.push_back(pivot + 1);
                indices.push_back(pivot + 3);
            }
        }


        return indices;
    }
}
