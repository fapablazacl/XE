
#include "IGraphicsContextES2.hpp"

namespace XE {
    IGraphicsContextES2::~IGraphicsContextES2() {}

    IGraphicsContextES2::Descriptor IGraphicsContextES2::Descriptor::defaultGL4()  {
        IGraphicsContextES2::Descriptor descriptor;

        descriptor.major = 4;
        descriptor.minor = 1;

        descriptor.coreProfile = true;
        descriptor.depthBits = 24;
        descriptor.redBits = 8;
        descriptor.greenBits = 8;
        descriptor.blueBits = 8;

        return descriptor;
    }
}
