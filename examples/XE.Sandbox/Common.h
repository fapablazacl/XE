
#pragma once

#include <cstring>
#include <iostream>
#include <XE/Math/Matrix.hpp>


inline std::ostream& indent(std::ostream &os, const int indentation) {
    for (int i=0; i<indentation; i++) {
        os << " ";
    }
    
    return os;
}


inline XE::Matrix4f makeMatrix(const float *data) {
    assert(data);
    
    XE::Matrix4f matrix;
    
    std::memcpy(matrix.data, data, 16 * sizeof(float));
    
    return matrix;
}
