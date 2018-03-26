
#ifndef __XE_PREDEF_HPP__
#define __XE_PREDEF_HPP__

#include <cstdint>

#if defined(__APPLE__)
namespace std {
    typedef uint8_t byte; 
}
#else 
#include <cstddef>
#endif

#endif
