#ifndef PX_INTRINSICS_H
#define PX_INTRINSICS_H

#include "Px.h"

#ifndef PX_PS3
	#error "This file should only be included by ps3 builds!!"
#endif

#include <math.h>
#ifdef __SPU__
#include "spu_intrinsics.h"
#else
#include "ppu_intrinsics.h"
#endif

namespace physx
{
namespace intrinsics
{
	//! \brief platform-specific absolute value
	PX_FORCE_INLINE float abs(float a)						{	return ::fabsf(a);	}

	//! \brief platform-specific select float
#ifdef __SPU__
	PX_FORCE_INLINE float fsel(float a, float b, float c)	{	return (a>=0 ? b : c);	}
#else
	PX_FORCE_INLINE float fsel(float a, float b, float c)	{	return __fsels(a, b, c);	}
#endif

	//! \brief platform-specific sign
#ifdef __SPU__
	PX_FORCE_INLINE float sign(float a)						{	return (a>=0 ? 1.0f : -1.0f); }
#else
	PX_FORCE_INLINE float sign(float a)						{	return __fsels(a, 1.0f, -1.0f); }
#endif

	//! \brief platform-specific reciprocal
	PX_FORCE_INLINE float recip(float a)					{	return 1.0f/a;			}

	//! \brief platform-specific reciprocal estimate
#if defined(__SPU__) || !defined(_PPU_INTRINSICS_GCC_H)
	PX_FORCE_INLINE float recipFast(float a)				{	return 1.0f/a;		}
#else
	PX_FORCE_INLINE float recipFast(float a)				{	return __fres(a);	}
#endif

	//! \brief platform-specific square root
	PX_FORCE_INLINE float sqrt(float a)						{	return ::sqrtf(a);	}

	//! \brief platform-specific reciprocal square root
	PX_FORCE_INLINE float recipSqrt(float a)				{   return 1.0f/::sqrtf(a); }

	//! \brief platform-specific reciprocal square root estimate
#ifdef __SPU__
	PX_FORCE_INLINE float recipSqrtFast(float a)			{	return 1.0f/::sqrtf(a);	}
#else
	PX_FORCE_INLINE float recipSqrtFast(float a)			{	return float(__frsqrte(a));	}
#endif

	//! \brief platform-specific sine
	PX_FORCE_INLINE float sin(float a)						{   return ::sinf(a); }

	//! \brief platform-specific cosine
	PX_FORCE_INLINE float cos(float a)						{   return ::cosf(a); }

	//! \brief platform-specific minimum
#ifdef __SPU__
	PX_FORCE_INLINE float selectMin(float a, float b)		{	return (a>=b ? b : a);	}
#else
	PX_FORCE_INLINE float selectMin(float a, float b)		{	return __fsels(a-b, b, a);	}
#endif

	//! \brief platform-specific maximum
#ifdef __SPU__
	PX_FORCE_INLINE float selectMax(float a, float b)		{	return (a>=b ? a : b); }
#else
	PX_FORCE_INLINE float selectMax(float a, float b)		{	return __fsels(a-b, a, b); }
#endif
	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_FORCE_INLINE bool isFinite(float a)					
	{	
		return !isnan(a) && !isinf(a); 
	}

	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_FORCE_INLINE bool isFinite(double a)					
	{	
		return !isnan(a) && !isinf(a); 
	}

} // namespace intrinsics
} // namespace physx

#endif //PX_INTRINSICS_H

