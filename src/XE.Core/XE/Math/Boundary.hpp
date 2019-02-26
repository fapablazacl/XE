
#ifndef __XE_MATH_BOUNDARY_HPP__
#define __XE_MATH_BOUNDARY_HPP__

#include <limits>
#include <cassert>

#include "Vector.hpp"

namespace XE {
    template<typename T, int N>
    struct Boundary {
        Vector<T, N> minEdge;
        Vector<T, N> maxEdge;

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

    public:
        Boundary() {
            minEdge = Vector<T, N>(std::numeric_limits<T>::max());
            maxEdge = Vector<T, N>(-std::numeric_limits<T>::max());
        }

        Boundary(const Vector<T, N> &value1, const Vector<T, N> &value2) {
            expand(value1);
            expand(value2);
        }

        void expand(const Vector<T, N> &value) {
            minEdge = Minimize(minEdge, value);
            maxEdge = Maximize(maxEdge, value);
        }

        void expand(const Boundary<T, N>& other) {
            expand(other.minEdge);
            expand(other.maxEdge);
        }

        Vector<T, N> computeSize() const {
            assert(isValid());

            return maxEdge - minEdge;
        }

        Vector<T, N> computeCenter() const {
            assert(isValid());

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
            assert(isValid());

            for(int i=0; i<N; ++i) { 
                T value = point[i];
        
                if (value < minEdge[i]) {
                    return false;

                } else if (value > maxEdge[i]) {
                    return false;
                }
            }
    
            return true;
        }

        Vector<T, N> getEdge(int pointIndex) const {
            assert(isValid());

            // FIXME
            const int LastPoint = Boundary<T, N>::PointCount-1;
            const Vector<T, N> *edges = &minEdge;   

            switch (pointIndex) {
                case 0:         return edges[0];
                case LastPoint: return edges[1];
        
                default: {
                    Vector<T, N> point;
                    int remainder;
                    
                    for(int i=0; i<N; ++i) {
                        remainder = pointIndex % 2;
                        pointIndex /= 2;
                
                        point[i] = edges[remainder][i];
                    }
            
                    return point;
                }
            }
        }

        bool intersect(const Boundary<T, N>& other) const {
            assert(isValid());

            return intersectImpl(other) || intersectImpl(*this);
        }
        
    private:
        bool intersectImpl(const Boundary<T, N>& other) const {
            assert(isValid());
            
            for(int i=0; i<Boundary<T, N>::PointCount; ++i) {
                if (this->isInside(other.getEdge(i))) {
                    return true;
                }
            }
            
            return false;
        }
    };
}

#endif 
