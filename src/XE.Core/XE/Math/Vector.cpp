
#include "Vector.hpp"

namespace XE {
    template struct Vector<std::int32_t, 2>;
    template struct Vector<std::int32_t, 3>;
    template struct Vector<std::int32_t, 4>;

    template struct Vector<float, 2>;
    template struct Vector<float, 3>;
    template struct Vector<float, 4>;

    template struct Vector<double, 2>;
    template struct Vector<double, 3>;
    template struct Vector<double, 4>;
}
