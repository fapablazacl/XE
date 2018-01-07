
#ifndef __XE_MATH_QUATERNION_HPP__
#define __XE_MATH_QUATERNION_HPP__

#include <cmath>
#include <cstdint>
#include <cassert>

#include "Vector.hpp"

namespace XE::Math {
    template<typename Type>
    struct Rotation {
        Type angle = Type(0);
        Vector<Type, 3> axis = {Type(0), Type(0), Type(0)};
    };

    template<typename T>
    struct Quaternion {
        union {
            struct {
                Vector<T, 3> V;
                T W;
            };

            T Data[4];
        };

        Quaternion() {}
        
        explicit Quaternion(const T *values) {
            assert(values);

            for (int i=0; i<4; i++) {
                Data[i] = values[i];
            }
        }
        
        Quaternion(const T x, const T y, const T z, const T w) {
            V.X = x;
            V.Y = y;
            V.Z = z;
            W = w;
        }
        
        explicit Quaternion(const Vector<T, 3> &v) {
            V = v;
            W = T(0);
        }
        
        Quaternion(const Vector<T, 3> &v, T w) {
            V = v;
            W = w;
        }

        Quaternion(const Vector<T, 4> &v) {
            V.X = v.X;
            V.Y = v.Y;
            V.Z = v.Z;
            W = v.W;
        }

        operator Rotation<T>() const {
            const T angle = T(2) * std::acos(w);

            if (angle == T(0)) {
                return {angle, {T(1), T(0), T(0)}};
            } else {    
                return {angle, normalize(v)};
            }
        }
        
        bool operator== (const Quaternion<T> &other) const {
            return q == other.q;
        }
        
        bool operator!= (const Quaternion<T> &other) const {
            return !(*this == other);
        }
        
        static Quaternion<T> makeZero() {
            return Quaternion<T>({T(0), T(0), T(0)}, T(0));
        }
        
        static Quaternion<T> makeIdentity() {
            return Quaternion<T>({T(0), T(0), T(0)}, T(1));
        }
        
        static Quaternion<T> makeRotation(const T radians, const Vector<T, 3> &axis) {
            auto q = Quaternion<T>(axis, std::cos(radians / T(2)));
            
            return normalize(q);
        }
        
        static Quaternion<T> makeRotation(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
            auto v = cross(v1, v2);
            auto w = std::sqrt(dot(v1, v1) * dot(v2, v2)) + dot(v1, v2);
            
            return normalize(Quaternion<T>(v, w));
        }
    };
    
    template<typename T>
    Quaternion<T> operator+ (const Quaternion<T> &q1, const Quaternion<T> &q2) {
        return {q1.v + q2.v, q1.w + q2.w};
    }
    
    template<typename T>
    Quaternion<T> operator- (const Quaternion<T> &q1, const Quaternion<T> &q2) {
        return {q1.v - q2.v, q1.w - q2.w};
    }
    
    template<typename T>
    Quaternion<T> operator- (const Quaternion<T> &q) {
        return {-q.v, -q.w};
    }
    
    template<typename T>
    Quaternion<T> operator+ (const Quaternion<T> &q) {
        return q;
    }
    
    template<typename T>
    Quaternion<T> operator* (const Quaternion<T> &q1, const Quaternion<T> &q2) {
        return {
            cross(q1.v, q2.v) + q2.v*q1.w + q1.v*q2.w,
            q1.w*q2.w - dot(q1.v, q2.v)
        };
    }
    
    template<typename T>
    Quaternion<T> operator / (const Quaternion<T> &q1, const Quaternion<T> &q2) {
        return q1 * inverse(q2);
    }
    
    template<typename T>
    Quaternion<T> operator * (const Quaternion<T> &q, const T s) {
        return {q.v*s, q.w*s};
    }
    
    template<typename T>
    Quaternion<T> operator * (const T s, const Quaternion<T> &q) {
        return q * s;
    }
    
    template<typename T>
    Quaternion<T> operator / (const Quaternion<T> &q, const T s) {
        return q * (T(1) / s);
    }
    
    template<typename T>
    Quaternion<T>& operator *= (Quaternion<T> &q, const Quaternion<T> &other) {
        return q = q * other;
    }
    
    template<typename T>
    Quaternion<T> inverse(const Quaternion<T> &q) {
        return conj(q) * norm_pow2(q);
    }

    template<typename T>
    Quaternion<T> conj(const Quaternion<T> &q) {
        return {-q.v, q.w};
    }
    
    template<typename T>
    T dot(const Quaternion<T> &q1, const Quaternion<T> &q2) {
        return dot(q1.q, q2.q);
    }
    
    template<typename T>
    T norm_pow2(const Quaternion<T> &q) {
        return dot(q, q);
    }

    template<typename T>
    T norm(const Quaternion<T> &q) {
        return std::abs(norm_pow2(q));
    }

    template<typename T>
    Quaternion<T> normalize(const Quaternion<T> &q) {
        return q / norm(q);
    }

    template<typename T>
    Vector<T, 3> transform (const Quaternion<T> &q, const Vector<T, 3> &v) {
        auto quat = q * Quaternion<T>(v) * inverse(q);
        return quat.v;
    }

    template<typename T>
    std::string to_string(const Quaternion<T> &q) {
        return to_string(q.q);
    }
}

#endif
