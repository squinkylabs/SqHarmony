
/**
 * include the "foreign" SIMD libraries (from VCV)
 */

// Need to make this compile in MS tools for unit tests
#if defined(_MSC_VER)

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#define NOMINMAX
#endif

// #include "SqMath.h"
#include <algorithm>
#include <cstdint>
#include <simd/Vector.hpp>
#include <simd/functions.hpp>

using float_4 = rack::simd::float_4;
using int32_4 = rack::simd::int32_4;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif