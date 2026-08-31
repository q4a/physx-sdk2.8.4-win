#ifndef PXC_MATH_WIN_H
#define PXC_MATH_WIN_H

#include <math.h>

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcSin(PxFloat s)
{
	return ::sinf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcCos(PxFloat s)
{
	return ::cosf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcTan(PxFloat s)
{
	return ::tanf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcASin(PxFloat s)
{
	if(s > 1.0f) s = 1.0f;
	else if(s < -1.0f) s = -1.0f;
	return ::asinf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcACos(PxFloat s)
{
	if(s > 1.0f) s = 1.0f;
	else if(s < -1.0f) s = -1.0f;
	return ::acosf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcFloor(PxFloat a)
{
	return ::floorf(a);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcCeil(PxFloat a)
{
	return ::ceilf(a);
}

template<class T>
CUDA_CALLABLE PXD_FORCE_INLINE T PxcClamp(T v, T hi, T low)
{
	return ((hi < v) ? 
				hi : 
				((v < low) ?
					low : v));
	/*if (hi < v) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;*/
}

template<class T>
CUDA_CALLABLE PXD_FORCE_INLINE T PxcAbs(T a)
{
	return (a >= 0) ? a : -a;
}

/*!
Return biggest of two entries.

Requires operator< on T
*/
template<class T>
CUDA_CALLABLE PXD_FORCE_INLINE T PxcMax(const T& a, const T& b)
{
	return (a < b ? b : a);
}
	
/*!
Return smaller of two entries.

Requires operator< on T
*/
template<class T>
CUDA_CALLABLE PXD_FORCE_INLINE T PxcMin(const T& a, const T& b)
{
	return (b < a ? b : a);
}

/*!
Specialize PxcMin and PxcMax for float to generate GPU's FMIN and FMAX instructions.
*/
#ifdef __CUDACC__
template<>
CUDA_CALLABLE PXD_FORCE_INLINE float PxcMax<float>(const float& a, const float& b)
{
	return fmaxf(a,b);
}

template<>
CUDA_CALLABLE PXD_FORCE_INLINE float PxcMin<float>(const float& a, const float& b)
{
	return fminf(a,b);
}
#endif // __CUDACC__

template<>
CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcAbs(PxFloat s)
{
	return ::fabsf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcSqrt(PxFloat s)
{
	return ::sqrtf(s);
}
CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcInvSqrt(PxFloat s)
{
	return 1/::sqrtf(s);
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcSign(PxFloat s)
{
	return (s >= 0.0f) ? 1.0f : -1.0f;
}

CUDA_CALLABLE PXD_FORCE_INLINE bool PxcIsFinite(PxFloat f)
{
#if defined(PXD_COMPILER_CUDA)
	return isfinite(f);
#else
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
#endif
}

#endif
