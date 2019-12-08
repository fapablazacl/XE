
#include "glad.h"

#if defined(_DEBUG)
  #include "glad_debug.c.in"
#else 
  #include "glad_release.c.in"
#endif
