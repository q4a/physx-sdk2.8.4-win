#ifndef PXC_MATH_XBOX_H
#define PXC_MATH_XBOX_H

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

/* 
Specializations for xbox360, xbox360 _really_ does not like float compares.

TODO: as we get more xbox specific functions break out into another header perhaps?
*/

template<>
PXD_FORCE_INLINE PxFloat PxcAbs(PxFloat s)
{
	return __fabs(s);
}

PXD_FORCE_INLINE PxFloat PxcSqrt(PxFloat s)
{
	return __fsqrts(s);
}

PXD_FORCE_INLINE PxFloat PxcSign(PxFloat s)
{
	return (PxFloat)__fsel(s, 1.0f, -1.0f);
}


template<>
CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcClamp(PxFloat v, PxFloat hi, PxFloat low)
{
	PxdF64 r = fpmax(v,low);
	r = fpmin(r,hi);

	return (PxFloat)r;
}

template<>
PXD_FORCE_INLINE PxdF64 PxcMax(const PxdF64& a, const PxdF64& b)
{
	return fpmax(a,b);
}

template<>
PXD_FORCE_INLINE PxFloat PxcMax(const PxFloat& a, const PxFloat& b)
{
	return (PxFloat)fpmax(a,b);
}

template<>
PXD_FORCE_INLINE PxdF64 PxcMin(const PxdF64& a, const PxdF64& b)
{
	return fpmin(a,b);
}

template<>
PXD_FORCE_INLINE PxFloat PxcMin(const PxFloat& a, const PxFloat& b)
{
	return (PxFloat)fpmin(a, b);
}

CUDA_CALLABLE PXD_FORCE_INLINE bool PxcIsFinite(PxFloat f)
{
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
}

#endif
