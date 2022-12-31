
#ifndef __XE_GRAPHICS_PLANEGENERATOR_HPP__
#define __XE_GRAPHICS_PLANEGENERATOR_HPP__

#include <vector>
#include <xe/math.h>

#include "ShapeGenerator.h"

namespace XE {
    template<typename FloatT>
    inline FloatT ratio(const int a, const int b) {
        return static_cast<FloatT>(a) / static_cast<FloatT>(b);
    }

    /**
     * @brief Generate a geometry mesh over the plane XZ
     */

    // TODO: Add the following generation parameters: Reference Plane, Rotation
    // TODO: Add a method to return required rendering parameters via the SubsetEnvelope class
    // TODO: Add control parameter for the Face Vertex Ordering
    template<typename T>
    class PlaneGenerator {
    public:
        PlaneGenerator(const Vector2i &division, const Vector<T, 2> &size, const Plane<T> &plane) {
            assert(division.X > 0);
            assert(division.Y > 0);
            assert(size.X > 0.0f);
            assert(size.Y > 0.0f);

            this->division = division;
            this->size = size;
            this->plane = plane;

            this->vertexCount = this->computeVertexCount();
            this->indexCount = this->computeIndexCount();
        }

    public:
        void generate(Vector<T, 3> *coordinates, Vector<T, 3> *normals, Vector<T, 2> *textureCoordinates) const {
            const int slices = division.X;
            const int stacks = division.Y;

            int vertexIndex = 0;

            for (int i=0; i<slices + 1; i++) {
                const auto ti = ratio<T>(i, slices);

                for (int j=0; j<stacks + 1; j++) {
                    const auto tj = ratio<T>(j, stacks);

                    if (coordinates) {
                        coordinates[vertexIndex] = this->computeCoordinate(ti, tj);
                    }

                    if (normals) {
                        normals[vertexIndex] = this->computeNormal(ti, tj);
                    }

                    if (textureCoordinates) {
                        textureCoordinates[vertexIndex] = this->computeTextureCoordinate(ti, tj);
                    }

                    ++vertexIndex;
                }
            }
        }

        template<typename I>
        void generateIndices(I *indices) const {
            const int slices = division.X;
            const int stacks = division.Y;
            const int slices_plus_1 = slices + 1;

            int index = 0;

            for (int i=0; i<slices; i++) {
                for (int j=0; j<stacks; j++) {
                    const int p0 = (i + 0) + (j + 0) * (slices_plus_1);
                    const int p1 = (i + 1) + (j + 0) * (slices_plus_1);
                    const int p2 = (i + 0) + (j + 1) * (slices_plus_1);
                    const int p3 = (i + 1) + (j + 1) * (slices_plus_1);

                    indices[index++] = p0;
                    indices[index++] = p1;
                    indices[index++] = p2;

                    indices[index++] = p1;
                    indices[index++] = p3;
                    indices[index++] = p2;
                }
            }
        }

        int getVertexCount() const {
            return vertexCount;
        }

        int getIndexCount() const {
            return indexCount;
        }

    private:
        Vector<T, 3> computeCoordinate(const T ti, const T tj) const {
            return {
                lerp(T(-0.5), T(0.5), tj), 
                lerp(T(0.5), T(-0.5), ti), 
                T(0.0)
            };
        }

        Vector<T, 3> computeNormal(const T ti, const T tj) const {
            return { T(0), T(0), T(-1) };
        }

        Vector<T, 2> computeTextureCoordinate(const T ti, const T tj) const {
            return {
                lerp(T(0), T(1), tj),
                lerp(T(1), T(0), ti)
            };
        }

        int computeVertexCount() const {
            const int slices = division.X;
            const int stacks = division.Y;

            return (slices + 1) * (stacks + 1);
        }

        int computeIndexCount() const {
            const int slices = division.X;
            const int stacks = division.Y;

            return 6 * (slices) * (stacks);
        }

    private:
        Vector2i division; 
        Vector<T, 2> size;
        Plane<T> plane;

        int vertexCount;
        int indexCount;
    };
}

#endif
