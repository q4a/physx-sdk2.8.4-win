#ifndef NX_FOUNDATION_NXMATH_FLASH
#define NX_FOUNDATION_NXMATH_FLASH
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
#include <cmath>

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

extern "C" bool isFinf(NxF32 f);
extern "C" bool isFin(NxF64 f);
NX_INLINE bool NxMath::isFinite(NxF32 f)
{
	return isFinf(f);
	//int result;
	//__asm("%[RES] = ((%[FARG] * 0) != 0) ? 0 : 1;//__isfinitef" : [RES] "=r" (result) : [FARG] "f" (f));
	//return (bool)result;
}

NX_INLINE bool NxMath::isFinite(NxF64 f)
{
	return isFin(f);
    //int result;
	//__asm("%[RES] = ((%[FARG] * 0) != 0) ? 0 : 1;//__isfinitef" : [RES] "=r" (result) : [FARG] "f" (f));
	//return (bool)result;
}


#endif
//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
