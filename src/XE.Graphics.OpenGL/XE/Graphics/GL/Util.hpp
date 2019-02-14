
#ifndef __XE_GRAPHICS_GL_UTIL_HPP__
#define __XE_GRAPHICS_GL_UTIL_HPP__

namespace XE {
    extern void __checkErrorImpl(const char *file, const int line);
}

#if defined(_DEBUG)
#  define XE_GRAPHICS_GL_CHECK_ERROR()  __checkErrorImpl(__FILE__, __LINE__)
#else 
#  define XE_GRAPHICS_GL_CHECK_ERROR()  do{} while(false)
#endif

#endif
