
#ifndef __XE_CORE_HPP__
#define __XE_CORE_HPP__

#include <XE/Predef.h>

namespace XE {
    struct Version {
        int major, minor, revision;
    };

    /**
     * @brief The Core Root class
     */ 
    class XE_API Core {
    public:
        Version GetVersion() const;
    };
}

#endif
