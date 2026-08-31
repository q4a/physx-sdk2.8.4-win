#ifndef NX_FOUNDATION_NXMATH_LINUX
#define NX_FOUNDATION_NXMATH_LINUX
/*----------------------------------------------------------------------------*\
|
|					Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

#if defined(__QNX__)
#include <math.h>
#else
#include <cmath>
#endif

NX_INLINE NxF32 NxMath::max(NxF32 a,NxF32 b)
{
	return (a < b) ? b : a;
}

NX_INLINE NxF64 NxMath::max(NxF64 a,NxF64 b)
{
	return (a < b) ? b : a;
}

NX_INLINE NxF32 NxMath::min(NxF32 a,NxF32 b)
{
	return (a < b) ? a : b;
}

NX_INLINE NxF64 NxMath::min(NxF64 a,NxF64 b)
{
	return (a < b) ? a : b;
}

NX_INLINE NxF32 NxMath::sqrt(NxF32 a)
{
	return ::sqrtf(a);
}

NX_INLINE NxF64 NxMath::sqrt(NxF64 a)
{
	return ::sqrt(a);
}

NX_INLINE bool NxMath::isFinite(NxF32 f)
{
#if defined(__QNX__)
	return isfinite(f);
#else
	return std::isfinite(f);
#endif
}

NX_INLINE bool NxMath::isFinite(NxF64 f)
{
#if defined(__QNX__)
	return isfinite(f);
#else
	return std::isfinite(f);
#endif
}


#endif
//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND