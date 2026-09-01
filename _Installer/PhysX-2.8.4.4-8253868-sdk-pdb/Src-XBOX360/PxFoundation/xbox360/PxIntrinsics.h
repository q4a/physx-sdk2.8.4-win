#ifndef PX_INTRINSICS_H
#define PX_INTRINSICS_H

#include "Px.h"

#ifndef _XBOX
	#error "This file should only be included by xbox builds!!"
#endif

#include <PPCIntrinsics.h>
#include <math.h>
#include <float.h>

namespace physx
{
namespace intrinsics
{
	//! \brief platform-specific absolute value
	PX_FORCE_INLINE float abs(float a)								{	return __fabs(a);	}

	//! \brief platform-specific select float
	PX_FORCE_INLINE float fsel(float a, float b, float c)			{	return __fself(a, b, c);	}

	//! \brief platform-specific sign
	PX_FORCE_INLINE float sign(float a)								{	return __fself(a, 1.0f, -1.0f); }

	//! \brief platform-specific reciprocal
	PX_FORCE_INLINE float recip(float a)							{	return 1.0f/(a);	}

	//! \brief platform-specific reciprocal estimate
	PX_FORCE_INLINE float recipFast(float a)						{	return __fres(a);	}

	//! \brief platform-specific square root
	PX_FORCE_INLINE float sqrt(float a)								{	return __fsqrts(a);	}

	//! \brief platform-specific reciprocal square root
	PX_FORCE_INLINE float recipSqrt(float a)						{   return recip(__fsqrts(a)); }

	//! \brief platform-specific reciprocal square root estimate
	PX_FORCE_INLINE float recipSqrtFast(float a)					{   return float(__frsqrte(a)); }

	//! \brief platform-specific sine
	PX_FORCE_INLINE float sin(float a)								{   return ::sinf(a); }

	//! \brief platform-specific cosine
	PX_FORCE_INLINE float cos(float a)								{   return ::cosf(a); }

	//! \brief platform-specific minimum
	PX_FORCE_INLINE float selectMin(float a, float b)				{	return __fself(a-b, b, a); }

	//! \brief platform-specific maximum
	PX_FORCE_INLINE float selectMax(float a, float b)				{	return __fself(a-b, a, b); }

	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_FORCE_INLINE bool isFinite(float a)
	{
		return 0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a));
	}

	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_FORCE_INLINE bool isFinite(double a)
	{
		return 0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a));
	}
} // namespace intrinsics
} // namespace physx
#endif
