
#include "Vector.hpp"
#include <cassert>

namespace XE::Math {
    template<typename T, int N>
    Vector<T, N>::Vector() {}

    template<typename T, int N>
    Vector<T, N>::Vector(T value) {
        for (T &element : this->Data) {
            element = value;
        }
    }

    template<typename T, int N>
    Vector<T, N>::Vector(const T *values) {
        assert(values);

        for (int i=0; i<N; i++) {
            this->Data[i] = values[i];
        }
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator+ () const {
        return *this;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator- () const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = -this->Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator+ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] + rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator- (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] - rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] * rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] / rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] * rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] / rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator+= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] += rhs.Data[i];
        }

        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator-= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] -= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] *= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] /= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] *= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] /= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator== (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->Data[i] != rhs.Data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator!= (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->Data[i] == rhs.Data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<> struct Vector<std::int32_t, 2>;
    template<> struct Vector<std::int32_t, 3>;
    template<> struct Vector<std::int32_t, 4>;

    template<> struct Vector<float, 2>;
    template<> struct Vector<float, 3>;
    template<> struct Vector<float, 4>;

    template<> struct Vector<double, 2>;
    template<> struct Vector<double, 3>;
    template<> struct Vector<double, 4>;
}
