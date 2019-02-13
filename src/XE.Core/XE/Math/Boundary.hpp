
#ifndef __XE_MATH_BOUNDARY_HPP__
#define __XE_MATH_BOUNDARY_HPP__

#include <limits>
#include <cassert>

#include "Vector.hpp"

namespace XE {
    template<typename T, int N>
    struct Boundary {
        Vector<T, N> MinEdge;
        Vector<T, N> MaxEdge;

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
            MinEdge = Vector<T, N>(std::numeric_limits<T>::max());
            MaxEdge = Vector<T, N>(-std::numeric_limits<T>::max());
        }

        Boundary(const Vector<T, N> &value1, const Vector<T, N> &value2) {
            Expand(value1);
            Expand(value2);
        }

        void Expand(const Vector<T, N> &value) {
            MinEdge = Minimize(MinEdge, value);
            MaxEdge = Maximize(MaxEdge, value);
        }

        void Expand(const Boundary<T, N>& other) {
            Expand(other.MinEdge);
            Expand(other.MaxEdge);
        }

        Vector<T, N> ComputeSize() const {
            assert(IsValid());

            return MaxEdge - MinEdge;
        }

        Vector<T, N> ComputeCenter() const {
            assert(IsValid());

            return MinEdge + ((MaxEdge - MinEdge) / T(2));
        }

        bool IsValid() const {
            for (int i=0; i<N; i++) {
                if (MinEdge[i] > MaxEdge[i]) {
                    return false;
                }
            }

            return true;
        }

        bool IsInside(const Vector<T, N> &point) const {
            assert(IsValid());

            for(int i=0; i<N; ++i) { 
                T value = point[i];
        
                if (value < MinEdge[i]) {
                    return false;

                } else if (value > MaxEdge[i]) {
                    return false;
                }
            }
    
            return true;
        }

        Vector<T, N> GetEdge(int pointIndex) const {
            assert(IsValid());

            // FIXME
            const int LastPoint = Boundary<T, N>::PointCount-1;
            const Vector<T, N> *edges = &MinEdge;   

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

        bool Intersect(const Boundary<T, N>& other) const {
            assert(IsValid());

            return IntersectImpl(other) || IntersectImpl(*this);
        }
        
    private:
        bool IntersectImpl(const Boundary<T, N>& other) const {
            assert(IsValid());
            
            for(int i=0; i<Boundary<T, N>::PointCount; ++i) {
                if (this->IsInside(other.GetEdge(i))) {
                    return true;
                }
            }
            
            return false;
        }
    };
}

#endif 
