#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG

#include <assert.h>

#define ASSERT(x)   assert(x)

#else

#define ASSERT(x)

#endif


#endif
