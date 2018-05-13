
#include "Core.hpp"

namespace XE {
    Version Core::GetVersion() const {
        return { XE_VERSION_MAJOR, XE_VERSION_MINOR, XE_VERSION_REVISION };
    }
}
