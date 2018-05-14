
#ifndef __XE_CORE_HPP__
#define __XE_CORE_HPP__

#include <XE/Predef.hpp>
#include <cstddef>

namespace XE {
    struct Version {
        int major, minor, revision;
    };

    /**
     * @brief The main coordinator for the rest of the subsystems
     */ 
    class XE_API Core {
    public:
        Version GetVersion() const;
    };
}

#endif
