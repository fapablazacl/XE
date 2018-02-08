
#ifndef __glad_glad_h__
#define __glad_glad_h__

#if defined(_DEBUG)
  #include "glad_debug.h"
#else 
  #include "glad_release.h"
#endif

// because Windows.h defines a max and a min macro, we undefine them
#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

#endif
