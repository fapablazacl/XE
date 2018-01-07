
#ifndef __XE_MATH_QUATERNION_HPP__
#define __XE_MATH_QUATERNION_HPP__

#include <cmath>
#include <cstdint>
#include <cassert>

#include "Vector.hpp"

namespace XE::Math {
    template<typename Type>
    struct Rotation {
        Type angle;
        Vector<Type, 3> axis;
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

        explicit Quaternion(const Vector<T, 4> &v) {
            V.X = v.X;
            V.Y = v.Y;
            V.Z = v.Z;
            W = v.W;
        }

        explicit operator Rotation<T>() const {
            const T angle = T(2) * std::acos(W);

            if (angle == T(0)) {
                return {angle, {T(1), T(0), T(0)}};
            } else {    
                return {angle, normalize(V)};
            }
        }
        
        explicit operator Vector<T, 4>() const {
            return {V.X, V.Y, V.Z, V.W};
        }

        Quaternion<T> operator+ (const Quaternion<T> &rhs) const {
            Quaternion<T> result;

            for (int i=0; i<4; i++) {
                result.Data[i] = this->Data[i] + rhs.Data[i];
            }

            return result;
        }
        
        Quaternion<T> operator- (const Quaternion<T> &rhs) const {
            Quaternion<T> result;

            for (int i=0; i<4; i++) {
                result.Data[i] = this->Data[i] + rhs.Data[i];
            }

            return result;
        }
        
        Quaternion<T> operator- () const {
            Quaternion<T> result;

            for (int i=0; i<4; i++) {
                result.Data[i] = -this->Data[i];
            }

            return result;
        }
        
        Quaternion<T> operator+ () const {
            return *this;
        }
        
        Quaternion<T> operator* (const Quaternion<T> &rhs) const {
            return {
                Cross(V, rhs.V) + rhs.v*W + V*rhs.W,
                W*rhs.W - Dot(V, rhs.V)
            };
        }
        
        Quaternion<T> operator / (const Quaternion<T> &rhs) const {
            return (*this) * Inverse(rhs);
        }
        
        Quaternion<T> operator* (const T s) const {
            Quaternion<T> result;

            for (int i=0; i<4; i++) {
                result.Data[i] = this->Data[i] / s;
            }

            return result;
        }
        
        Quaternion<T> operator/ (const T s) const {
            Quaternion<T> result;

            for (int i=0; i<4; i++) {
                result.Data[i] = this->Data[i] / s;
            }

            return result;
        }
        
        friend Quaternion<T> operator* (const T s, const Quaternion<T> &q) {
            return q * s;
        }
        
        Quaternion<T>& operator+= (const Quaternion<T> &rhs) {
            for (int i=0; i<4; i++) {
                this->Data[i] += rhs.Data[i];
            }

            return *this;
        }

        Quaternion<T>& operator-= (const Quaternion<T> &rhs) {
            for (int i=0; i<4; i++) {
                this->Data[i] -= rhs.Data[i];
            }

            return *this;
        }

        Quaternion<T>& operator*= (const Quaternion<T> &rhs) {
            *this = *this * rhs;

            return *this;
        }

        Quaternion<T>& operator/= (const Quaternion<T> &rhs) {
            *this = *this / rhs;

            return *this;
        }

        Quaternion<T>& operator*= (const T s) {
            for (int i=0; i<4; i++) {
                this->Data[i] *= s;
            }

            return *this;
        }

        Quaternion<T>& operator/= (const T s) {
            for (int i=0; i<4; i++) {
                this->Data[i] /= s;
            }

            return *this;
        }

        bool operator== (const Quaternion<T> &rhs) const {
            for (int i=0; i<4; i++) {
                if (this->Data[i] != rhs.Data[i]) {
                    return false;
                }
            }

            return true;
        }
        
        bool operator!= (const Quaternion<T> &rhs) const {
            for (int i=0; i<4; i++) {
                if (this->Data[i] != rhs.Data[i]) {
                    return true;
                }
            }

            return false;
        }

        static Quaternion<T> Zero() {
            return Quaternion<T>({T(0), T(0), T(0)}, T(0));
        }
        
        static Quaternion<T> Identity() {
            return Quaternion<T>({T(0), T(0), T(0)}, T(1));
        }
        
        static Quaternion<T> Rotation(const T radians, const Vector<T, 3> &axis) {
            auto q = Quaternion<T>(axis, std::cos(radians / T(2)));
            
            return normalize(q);
        }
        
        static Quaternion<T> Rotation(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
            auto v = cross(v1, v2);
            auto w = std::sqrt(dot(v1, v1) * dot(v2, v2)) + dot(v1, v2);
            
            return normalize(Quaternion<T>(v, w));
        }
    };

    template<typename T>
    Quaternion<T> Inverse(const Quaternion<T> &q) {
        return conj(q) * norm_pow2(q);
    }

    template<typename T>
    Quaternion<T> Conjugate(const Quaternion<T> &q) {
        return {-q.V, q.W};
    }

    template<typename T>
    T Dot(const Quaternion<T> &q1, const Quaternion<T> &q2) {
        T sum = T(0);

        for (int i=0; i<4; i++) {
            sum += q1.Data[i]*q2.Data[i];
        }

        return sum;
    }

    template<typename T>
    T Abs2(const Quaternion<T> &q) {
        return Dot(q, q);
    }

    template<typename T>
    T Abs(const Quaternion<T> &q) {
        return static_cast<T>(std::sqrt(Abs2(q)));
    }

    template<typename T>
    Quaternion<T> Normalize(const Quaternion<T> &q) {
        return q / Abs(q);
    }

    template<typename T>
    Vector<T, 3> Transform(const Quaternion<T> &q, const Vector<T, 3> &v) {
        return (q * Quaternion<T>(v) * Inverse(q)).V;
    }
}

#endif
