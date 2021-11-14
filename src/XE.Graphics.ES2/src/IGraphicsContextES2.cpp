
#include <XE/Graphics/ES2/IGraphicsContextES2.h>

namespace XE {
    IGraphicsContextES2::~IGraphicsContextES2() {}
    
    IGraphicsContextES2::Descriptor IGraphicsContextES2::Descriptor::defaultES2()  {
        IGraphicsContextES2::Descriptor descriptor;

        descriptor.major = 2;
        descriptor.minor = 0;

        descriptor.coreProfile = true;
        descriptor.depthBits = 24;
        descriptor.redBits = 8;
        descriptor.greenBits = 8;
        descriptor.blueBits = 8;

        return descriptor;
    }
}
