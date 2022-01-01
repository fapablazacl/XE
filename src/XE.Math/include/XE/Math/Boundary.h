
#ifndef __XE_MATH_BOUNDARY_HPP__
#define __XE_MATH_BOUNDARY_HPP__

#include <limits>
#include <cassert>

#include "Vector.h"

namespace XE {
    //! N-dimensional boundary class
    //! Used as a basis for implementing boundary logic in specific dimensions, 
    //! like Rect (for 2-space), and Box (for 3-space).
    template<typename T, int N>
    class Boundary {
    public:
        template<int Base, int Exp> 
        struct Power {
            enum { Value = Base * Power<Base, Exp - 1>::Value };
        };
    
        template<int Base> 
        struct Power<Base, 0> {
            enum { Value = 1 };
        };

    public:
        enum { PointCount = Power<2, N>::Value };

        enum {
            MinEdge = 0,
            MaxEdge = PointCount - 1
        };

    public:
        Boundary() {}

        explicit Boundary(const Vector<T, N> &value) 
            : minEdge(value), maxEdge(value) {}

        Boundary(const Vector<T, N> &value1, const Vector<T, N> &value2) : Boundary(value1) {
            expand(value2);
        }

        void expand(const Vector<T, N> &value) {
            minEdge = minimize(minEdge, value);
            maxEdge = maximize(maxEdge, value);
        }

        void expand(const Boundary<T, N>& other) {
            expand(other.minEdge);
            expand(other.maxEdge);
        }

        Vector<T, N> getMinEdge() const {
            return minEdge;
        }

        Vector<T, N> getMaxEdge() const {
            return maxEdge;
        }

        Vector<T, N> getSize() const {
            return maxEdge - minEdge;
        }

        Vector<T, N> getCenter() const {
            return minEdge + ((maxEdge - minEdge) / T(2));
        }

        bool isValid() const {
            for (int i=0; i<N; i++) {
                if (minEdge[i] > maxEdge[i]) {
                    return false;
                }
            }

            return true;
        }

        bool isInside(const Vector<T, N> &point) const {
            for(int i=0; i<N; ++i) { 
                const T value = point[i];
        
                if (value < minEdge[i] || value > maxEdge[i]) {
                    return false;
                }
            }
    
            return true;
        }

        Vector<T, N> getEdge(int pointIndex) const {
            assert(pointIndex >= MinEdge);
            assert(pointIndex <= MaxEdge);

            if (pointIndex == MinEdge) {
                return minEdge;
            }

            if (pointIndex == MaxEdge) {
                return maxEdge;
            }

            const Vector<T, N> *edges = &minEdge;

            Vector<T, N> point;
            for(int i=0; i<N; ++i) {
                const int remainder = pointIndex % 2;
                pointIndex /= 2;
        
                point[i] = edges[remainder][i];
            }
    
            return point;
        }

        bool intersect(const Boundary<T, N>& other) const {
            return intersectImpl(other) || intersectImpl(*this);
        }
        
    private:
        bool intersectImpl(const Boundary<T, N>& other) const {
            for(int i=0; i<Boundary<T, N>::PointCount; ++i) {
                if (isInside(other.getEdge(i))) {
                    return true;
                }
            }
            
            return false;
        }

    private:
        Vector<T, N> minEdge;
        Vector<T, N> maxEdge;
    };
}

#endif 
