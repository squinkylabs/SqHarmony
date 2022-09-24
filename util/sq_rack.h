
#pragma once

/**
 * If this file is included instead of rack.hpp, then it will compile
 * in Visual studio and work with the unit tests.
 * 
 */
#include "AudioMath.h"
#include "SqLog.h"

#if defined(_MSC_VER)
#define __attribute__(x)
#pragma warning (push)
// turn off some MSVC warnings that are triggered by rack.hpp
#pragma warning(disable: 4244 4305 4267)


#define __ATOMIC_ACQUIRE 0
#define __ATOMIC_RELEASE 0

inline int  __atomic_add_fetch (int *, int, int) {
    SQWARN("__atomic_add_fetch 21");
    return 0;
}

inline size_t  __atomic_add_fetch (volatile size_t *, size_t, int) {
     SQWARN("__atomic_add_fetch 26");
    return 0;
}

inline size_t  __atomic_sub_fetch (volatile size_t *, size_t, int) {
     SQWARN("__atomic_sub_fetch");
    return 0;
}

// Some constants that VCV needs that MSVC doesn't have
#define M_PI AudioMath::Pi
#define M_SQRT2 1.41421356237309504880
#endif


#include "rack.hpp"
using float_4 = rack::simd::float_4;
using namespace rack::simd;

#if defined(_MSC_VER)
#pragma warning (pop)
#endif
