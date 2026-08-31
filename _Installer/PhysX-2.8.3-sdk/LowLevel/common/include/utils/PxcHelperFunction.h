#ifndef PXC_HELPERFUNCTION_H
#define PXC_HELPERFUNCTION_H

#include "px_config.h"
#include "PxcMath.h"
#include "PxnError.h"

#if defined(PXD_PLATFORM_XBOX360) || defined(PXD_PLATFORM_WINDOWS)
	#include "PxcPlatformInclude.h"
#endif

template<int A, int B>
struct PxcCtEQ
{
	enum
	{
		value = (A == B)
	};
};


// Some special trickery
#ifdef PXD_COMPILER_VC
	#if _MSC_VER < 1310	//MSVC prior to 7.1 does not like this
		#define PXC_NO_PARTIAL_SPECIALIZATION
	#endif
#endif


#ifndef PXC_NO_PARTIAL_SPECIALIZATION
template<unsigned int A, unsigned int B, bool Bis0>
struct PxcGCDCtHelper;

/*!
Helper class for implementation of GCD and LCM
*/
template<unsigned int A, unsigned int B>
struct PxcGCDCtHelper<A, B, false>
{
	static const unsigned int AmodB = A % B;
	enum
	{
		value = PxcGCDCtHelper<B, static_cast<unsigned int>(AmodB), PxcCtEQ<AmodB, 0>::value>::value
	};
};

/*!
Helper class for implementation of GCD and LCM
*/
template<unsigned int A, unsigned int B>
struct PxcGCDCtHelper<A, B, true>
{
	enum
	{
		value = A
	};
};

/*!
Compile time calculation of greatest common divisor.

\pre
A != 0
B != 0
*/
template<unsigned int A, unsigned int B>
class PxcGCDCt
{
public:
	enum { value = PxcGCDCtHelper<A, B, false>::value };
};

#else

/*!
	Helper class for implementation of GCD and LCM
*/
template<unsigned int A>
class PxcGCDCtHelper
{
public:
	template<unsigned int B>
	struct helper
	{
		enum
		{
			value = PxcGCDCtHelper<B>::helper<A%B>::value
		};
	};
	template<>
	struct helper<0>
	{
		enum
		{
			value = A
		};
	};
};


/*!
Compile time calculation of greatest common divisor.

\pre
A != 0
B != 0
*/
template<unsigned int A, unsigned int B>
class PxcGCDCt
{
public:
	enum { value = PxcGCDCtHelper<A>::helper<B>::value };
};
#endif


/*!
Compile time calculation of least common multiple.

\pre
A != 0
B != 0
*/
template<unsigned int A, unsigned int B>
class PxcLCMCt
{
public:
	enum { value = A / PxcGCDCt<A,B>::value * B };
};


/*!
Runtime calculation of greatest common divisor
*/
template<class Integer>
Integer PxcGDC(Integer A, Integer B)
{
	do {
		const Integer tmp(B);
		B = A % B;
		A = tmp;
	} while(B != 0);
	return A;
}

/*!
Runtime calculation of least common multiple
*/
template<class Integer>
Integer PxcLCM(Integer A, Integer B)
{
	return A / PxcGDC(A, B) * B;
}

/*!
Round up to next 32 boundary
*/
template<class T>
PXD_FORCE_INLINE T PxcCeil32(const T& x)
{
	return (x+31)&~31;
}

/*!
Rounding mode independent floor
*/
#if defined(PXD_COMPILER_VC) && !defined(_WIN64) && !defined(_M_ARM)
	
PXD_FORCE_INLINE PxI32 PxcFloorFast32(PxdF32 f)
{
	const PxdF32 h = 0.5f;
	PxI32 t = 0;
	_asm
	{
		fld		f
		fsub	h
		fistp	t
	};	
	return t;
}

#else
//we are not yet sure, whether this works for ps3
PXD_FORCE_INLINE PxI32 PxcFloorFast32(PxdF32 f)
{
	return (PxI32)PxcFloor(f);
}
#endif

PXD_FORCE_INLINE PxU32 PxcBitCount32(PxU32 v)
{
	// from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	unsigned int const w = v - ((v >> 1) & 0x55555555);
	unsigned int const x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
	return ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

PXD_FORCE_INLINE PxU32 PxcNextPower2(PxU32 v)
{
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	v |= (v >> 8);
	v |= (v >> 16);
	return v+1;
}

/*!
determines if v is a power of 2
*/
PXD_FORCE_INLINE bool PxcIsPower2(PxU32 v)
{
	// from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	return !(v & (v - 1)) && v;
}

/*!
Return the index of the lowest set bit. Or 0 if no bits are set.
*/
PXD_FORCE_INLINE PxU32 PxcLowestSetBit(PxU32 v)
{
	static const PxU32 MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	
	return MultiplyDeBruijnBitPosition[(((v & -v) * 0x077CB531UL) & 0xFFFFFFFFUL) >> 27];
}
/*!
Swap elements x and y
*/
template<class T>
PXD_FORCE_INLINE void PxcSwap(T& x, T& y)
{
	T tmp = x;
	x = y;
	y = tmp;
}

/*!
Sort two elements using operator<

On return x will be the smaller of the two
*/
template<class T>
PXD_FORCE_INLINE void PxcSort(T& x, T& y)
{
	if(y < x)
		PxcSwap(x, y);
}

/*!
Sort two elements using operator< and also keep order
of any extra data
*/
template<class T, class E1>
PXD_FORCE_INLINE void PxcSort(T& x, T& y, E1& xe1, E1& ye1)
{
	if(y < x)
	{
		PxcSwap(x, y);
		PxcSwap(xe1, ye1);
	}
}


/* 
Specializations for xbox360, xbox360 _really_ does not like float compares -> use select.
*/
#ifdef PXD_PLATFORM_XBOX360
#include "xbox360/PxcHelperfunction_XBOX.h"
#endif

//Tempory debug output...
PXD_FORCE_INLINE void PxcOutputDebugString(const char *str)
{
	PxnErrorReport(PXD_ERROR_INTERNAL_ERROR, "%s", str);
}

#endif
