
#include <iostream>

#include <XE/DataAttribute.hpp>
#include <XE/DataLayout.hpp>

int main(int argc, char **argv) {
    XE::DataLayout<XE::DataAttribute> layout = {
        {XE::DataType::Float32, 3, "Hola"}
    };

    return 0;
}
