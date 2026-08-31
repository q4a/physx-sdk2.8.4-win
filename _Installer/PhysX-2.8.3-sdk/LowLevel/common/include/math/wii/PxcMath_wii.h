#ifndef PXC_MATH_WII_H
#define PXC_MATH_WII_H

#include <math.h>
#define OPTIMIZE_FOR_WII

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

/* 
Specializations for xbox360, xbox360 _really_ does not like float compares.

TODO: as we get more xbox specific functions break out into another header perhaps?
*/

template<>
PXD_FORCE_INLINE PxFloat PxcAbs(PxFloat s)
{
	return __fabsf(s);
}

PXD_FORCE_INLINE PxFloat PxcSign(PxFloat s)
{
	register PxFloat r,rs = s, rone=1.0f, rnone=-1.0f;
	asm	{ fsel r,rs,rone,rnone }
	return r;
}

template<>
PXD_FORCE_INLINE PxdF64 PxcMax(const PxdF64& a, const PxdF64& b)
{
	register PxFloat r,c,ra=a,rb=b;
	c = ra-rb;
	asm	{ fsel r, c, ra, rb }
	return r;
}

template<>
PXD_FORCE_INLINE PxFloat PxcMax(const PxFloat& a, const PxFloat& b)
{
	register PxFloat r,c,ra=a,rb=b;
	c = ra-rb;
	asm	{ fsel r, c, ra, rb }
	return r;
}

template<>
PXD_FORCE_INLINE PxdF64 PxcMin(const PxdF64& a, const PxdF64& b)
{
	register PxFloat r,c,ra=a,rb=b;
	c = rb-ra;
	asm	{ fsel r, c, ra, rb }
	return r;
}

template<>
PXD_FORCE_INLINE PxFloat PxcMin(const PxFloat& a, const PxFloat& b)
{
	register PxFloat r,c,ra=a,rb=b;
	c = rb-ra;
	asm	{ fsel r, c, ra, rb }
	return r;
}

CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcSqrt(PxFloat s)
{
	return ::sqrtf(s);
}
CUDA_CALLABLE PXD_FORCE_INLINE PxFloat PxcInvSqrt(PxFloat s)
{
	register PxFloat fs = s, r, 
					nwork0, nwork1, 
					c_half = 0.5f, c_three = 3.0f;
					
	//Newton's refinement x 1
	//E' = (E/2)(3 - fa * E * E)
	asm	
	{ 
		frsqrte	r,fs;
		fmuls 	nwork0, r, r;
		fmuls 	nwork1, r, c_half;
		fnmsubs	nwork0, nwork0, fs, c_three;
		fmuls	r, nwork0, nwork1; 
	}
	return r;
}

	// Some float optimizations ported over from novodex.

	//returns non zero if the value is negative.
	#define PXC_IS_NEGATIVE(x) (((PxU32&)(x)) & 0x80000000)


CUDA_CALLABLE PXD_FORCE_INLINE bool PxcIsFinite(PxFloat f)
{
	return isfinite(f);
}

#endif

