// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef PX_FOUNDATION_PXINTRINSICS_H
#define PX_FOUNDATION_PXINTRINSICS_H

#include "Px.h"

#ifndef PX_WINDOWS
	#error "This file should only be included by Windows builds!!"
#endif

#include <math.h>
#include <float.h>

namespace physx
{
namespace intrinsics
{
	//! \brief platform-specific absolute value
	PX_CUDA PX_FORCE_INLINE float abs(float a)						{	return ::fabs(a);	}

	//! \brief platform-specific select float
	PX_CUDA PX_FORCE_INLINE float fsel(float a, float b, float c)	{	return (a >= 0.0f) ? b : c;	}

	//! \brief platform-specific sign
	PX_CUDA PX_FORCE_INLINE float sign(float a)						{	return (a >= 0.0f) ? 1.0f : -1.0f; }

	//! \brief platform-specific reciprocal
	PX_CUDA PX_FORCE_INLINE float recip(float a)					{	return 1.0f/a;			}

	//! \brief platform-specific reciprocal estimate
	PX_CUDA PX_FORCE_INLINE float recipFast(float a)				{	return 1.0f/a;			}

	//! \brief platform-specific square root
	PX_CUDA PX_FORCE_INLINE float sqrt(float a)						{	return ::sqrtf(a);	}

	//! \brief platform-specific reciprocal square root
	PX_CUDA PX_FORCE_INLINE float recipSqrt(float a)				{   return 1.0f/::sqrtf(a); }

	//! \brief platform-specific reciprocal square root estimate
	PX_CUDA PX_FORCE_INLINE float recipSqrtFast(float a)			{	return 1.0f/::sqrtf(a); }

	//! \brief platform-specific sine
	PX_CUDA PX_FORCE_INLINE float sin(float a)						{   return ::sinf(a); }

	//! \brief platform-specific cosine
	PX_CUDA PX_FORCE_INLINE float cos(float a)						{   return ::cosf(a); }

	//! \brief platform-specific minimum
	PX_CUDA PX_FORCE_INLINE float selectMin(float a, float b)		{	return a<b ? a : b;	}

	//! \brief platform-specific maximum
	PX_CUDA PX_FORCE_INLINE float selectMax(float a, float b)		{	return a>b ? a : b; }

	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_CUDA PX_FORCE_INLINE bool isFinite(float a)
	{
#ifdef __CUDACC__
		return isfinite(a) ? true : false;
#else
		return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a) ));
#endif
	}

	//! \brief platform-specific finiteness check (not INF or NAN)
	PX_CUDA PX_FORCE_INLINE bool isFinite(double a)
	{
#ifdef __CUDACC__
		return isfinite(a) ? true : false;
#else
		return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(a) ));
#endif
	}
} // namespace intrinsics
} // namespace physx

#endif
