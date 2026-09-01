#ifndef NX_FOUNDATION_NXMATH_PS3
#define NX_FOUNDATION_NXMATH_PS3
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

#ifndef __SPU__
#include <ppu_intrinsics.h>
#endif

NX_INLINE NxF32 NxMath::max(NxF32 a,NxF32 b)
{
#ifdef __SPU__
	return a>b ? a : b;
#else
	return __fsels(a-b, a, b);
#endif
}

NX_INLINE NxF64 NxMath::max(NxF64 a,NxF64 b)
{
#ifdef __SPU__
	return a>b ? a : b;
#else
	return __fsel(a-b, a, b);
#endif
}

NX_INLINE NxF32 NxMath::min(NxF32 a,NxF32 b)
{
#ifdef __SPU__
	return a<b ? a : b;
#else
	return __fsels(a-b, b, a);
#endif
}

NX_INLINE NxF64 NxMath::min(NxF64 a,NxF64 b)
{
#ifdef __SPU__
	return a<b ? a : b;
#else
	return __fsel(a-b, b, a);
#endif
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
	return (!(isnan(f)||isinf(f)));
}

NX_INLINE bool NxMath::isFinite(NxF64 f)
{
	return (!(isnan(f)||isinf(f)));
}

#endif
//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
