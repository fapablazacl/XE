
#ifndef __XE_CORE_HPP__
#define XE_CORE_HPP_

#include <xe/Predef.h>

namespace XE {
    struct Version {
        int major, minor, revision;
    };

    /**
     * @brief The Core Root class
     */
    class XE_API Core {
    public:
        static Version GetVersion() ;
    };
} // namespace XE

#endif
