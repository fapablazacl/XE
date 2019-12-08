
#include "IGraphicsContextGL.hpp"

namespace XE {
    IGraphicsContextGL::~IGraphicsContextGL() {}

    IGraphicsContextGL::Descriptor IGraphicsContextGL::Descriptor::defaultGL4()  {
        IGraphicsContextGL::Descriptor descriptor;

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
