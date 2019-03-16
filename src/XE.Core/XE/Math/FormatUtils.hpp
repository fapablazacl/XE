
#ifndef _XE_MATH_FORMATUTILS_HPP__
#define _XE_MATH_FORMATUTILS_HPP__

#include <ostream>
#include <sstream>
#include <string>
#include <iomanip>

namespace XE {
    template<typename T, int N>
    struct Vector;

    /**
     * @brief Convert a Vector into a JSON-like string representation
     */
    template<typename T, int N>
    std::string toString(const XE::Vector<T, N> &v, const int precision) {
        std::stringstream os;

        os << std::setprecision(precision);
        os << "{";

        for (int i=0; i<N; i++) {
            os << v[i];

            if (i < N - 1) {
                os << ", ";
            }
        }

        os << "}";
 
        return os.str();
    }

}

namespace std {
    /**
     * @brief Send a string-representation of the supplied row vector
     */
    template<typename T, int N>
    std::ostream& operator<< (std::ostream &os, const XE::Vector<T, N> &v) {
        os << "[";

        for (int i=0; i<N; i++) {
            os << v[i];

            if (i < N - 1) {
                os << ", ";
            }
        }

        os << "]";

        return os;
    }    
}


namespace XE {
    template<typename T, int R, int C>
    struct Matrix;

    /**
     * @brief Convert a Vector into a JSON-like string representation
     */
    template<typename T, int R, int C>
    std::string toString(const XE::Matrix<T, R, C>& m, const int precision) {
        std::string result;

        result += "{\n";

        for (int i=0; i<R; ++i) {
            result += "    " + toString(m.getRow(i), precision) + "\n";
        }

        result += "}";

        return result;
    }
}

namespace std {
    template<typename T, int R, int C>
    std::ostream& operator<< (std::ostream &os, const XE::Matrix<T, R, C>& m) {
        os << std::endl;

        for (int i=0; i<R; ++i) {
            os << m.getRow(i);
            os << std::endl;
        }

        os << std::endl;

        return os;
    }
}

#endif
