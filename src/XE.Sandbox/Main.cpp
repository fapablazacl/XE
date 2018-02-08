
#include <iostream>

#include <XE/DataAttribute.hpp>
#include <XE/DataLayout.hpp>
#include <XE/Graphics/GL/GraphicsDeviceGL.hpp>

int main(int argc, char **argv) {
    /*
    DataType Type;
    std::int16_t Dimension;
    char Name[16];
    */

    XE::DataLayout<XE::DataAttribute> layout = {
        {XE::DataType::Float32, 3, "Hola"}
    };

    return 0;
}
