#ifndef NX_SIMD_H
#define NX_SIMD_H

#include "Nx.h"
#include "NxVec3.h"
#include "NxPlane.h"
#include "NxMath.h"
#include <float.h>

/*!
NxSimd provides an abstraction of platform specific SIMD functionality. The aim is to
take advantage on SIMD on other platforms eventually, with less effort. (and aid testing)

Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. NxVec3s will be read/written as 4 component vectors!

NOTE: NxSimd mirrors PxcSimd. In future we should probably use the same source somehow.
*/

#if defined(WIN32) && defined(NX_SUPPORT_SSE)
#include "win/NxSimd_WIN.h"
#elif (defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__)) && defined(NX_SUPPORT_SSE)
#include "linux/NxSimd_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxSimd_XBOX.h"
#else
#define NX_USE_PLAIN_SIMD	//Define this to always force usage of C++ implementation
#endif

//! Plain C++ version
#ifdef NX_USE_PLAIN_SIMD
class NxSimd
#else
class NxSimdVerify
#endif
{
public:
	struct Vector4
	{
		Vector4()
		{}

		Vector4(NxReal x, NxReal y, NxReal z, NxReal w)
			: x(x), y(y), z(z), w(w)
		{}

		Vector4(NxU32 x, NxU32 y, NxU32 z, NxU32 w)
			: ux(x), uy(y), uz(z), uw(w)
		{}

		union
		{
			struct 
			{
				NxReal x, y, z, w;
			};
			struct 
			{
				NxU32 ux, uy, uz, uw;
			};
			NxReal f[4];
			NxU32	u[4];
		};
		
	};

	/* simd support */
	static NX_INLINE bool isSupported()
	{
		return true;
	}

	/* Setup any rounding modes etc */
	static NX_INLINE NxU32 setup()
	{
		return 0;
	}

	/* Reset any setup rounding modes etc*/
	static NX_INLINE void reset(NxU32 fromSetup)
	{
	}

	/* loads */	
	static NX_INLINE Vector4 load(const NxVec3 &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static NX_INLINE Vector4 loadW1(const NxVec3 &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 1.0f);
	}

	static NX_INLINE Vector4 load(const NxPlane &pxcPlane)
	{
		return Vector4(pxcPlane.normal.x, pxcPlane.normal.y, pxcPlane.normal.z, pxcPlane.d);
	}

	//A = aligned
	static NX_INLINE Vector4 loadA(const NxVec3 &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static NX_INLINE Vector4 load(const NxReal &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	static NX_INLINE Vector4 load(const NxU32 &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	static NX_INLINE Vector4 load(const PxU8 &a)
	{
		return Vector4((NxU32)a, 0, 0, 0);
	}

	static NX_INLINE Vector4 load3(const NxReal* a)
	{
		return Vector4(a[0], a[1], a[2], 0.0f);
	}

	static NX_INLINE Vector4 load4(const NxReal* a)
	{
		return Vector4(a[0], a[1], a[2], a[3]);
	}

	/* stores */
	static NX_INLINE void store(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

	//A = aligned
	static NX_INLINE void storeA(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

	static NX_INLINE void storeA4(NxVec3 &nxVec, const Vector4 &simdVec)
	{
		NxReal *fPtr = (NxReal *)&nxVec;
		fPtr[0] = simdVec.x;
		fPtr[1] = simdVec.y;
		fPtr[2] = simdVec.z;
		fPtr[3] = simdVec.w;
	}

	static NX_INLINE void store(NxReal &a, const Vector4 &simdVec)
	{
		a = simdVec.x;
	}

	static NX_INLINE void store(NxU32 &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}

	static NX_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}

	/*
	Conversion operators
	*/

	static NX_INLINE Vector4 uintToFloat(const Vector4 &a)
	{
		return Vector4((NxReal)a.ux,(NxReal)a.uy,(NxReal)a.uz,(NxReal)a.uw);
	}

	/* basic math */
	static NX_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
	}

	static NX_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
	}

	static NX_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
	}

	static NX_INLINE Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return add(multiply(a, b), c);
	}

	static NX_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return Vector4(1.0f/a.x, 1.0f/a.y, 1.0f/a.z, 1.0f/a.w);
	}

	static NX_INLINE Vector4 reciprocalEst(const Vector4 &a)
	{
		return reciprocal(a);
	}

	static NX_INLINE Vector4 reciprocalSafe(const Vector4 &a)
	{
		Vector4 mask = notEqual(a, zero());
		Vector4 result = reciprocal(a);
		return and4(result, mask);
	}

	static NX_INLINE Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return reciprocal(sqrt(a));
	}

	static NX_INLINE Vector4 pow(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(NxMath::pow(a.x, b.x), NxMath::pow(a.y, b.y), NxMath::pow(a.z, b.z), NxMath::pow(a.w, b.w));
	}

	static NX_INLINE Vector4 sqrt(const Vector4 &a)
	{
		return Vector4(NxMath::sqrt(a.x),NxMath::sqrt(a.y),NxMath::sqrt(a.z),NxMath::sqrt(a.w));
	}

	static NX_INLINE Vector4 sqrtSafe(const Vector4 &a)
	{
		return sqrt(a);
	}

	static NX_INLINE Vector4 acos(const Vector4 &a)
	{
		return Vector4(NxMath::acos(a.x), NxMath::acos(a.y), NxMath::acos(a.z), NxMath::acos(a.w));
	}

	// 3 component dot product
	static NX_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		NxReal d = a.x*b.x + a.y*b.y + a.z*b.z;
		return Vector4(d,d,d,d);
	}

	static NX_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		NxReal d = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
		return Vector4(d,d,d,d);
	}

	static NX_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.y*b.z - a.z*b.y,
						a.z*b.x - a.x*b.z,
						a.x*b.y - a.y*b.x,
						0);
	}

	static NX_INLINE Vector4 abs(const Vector4 &a)
	{
		return Vector4(NxMath::abs(a.x), NxMath::abs(a.y), NxMath::abs(a.z), NxMath::abs(a.w));
	}

	static NX_INLINE Vector4 magnitude(const Vector4 &a)
	{
		return sqrt(dot(a, a));
	}

	// magnitude with safe zero magnitude handling.
	static NX_INLINE Vector4 magnitudeSafe(const Vector4 &a)
	{
		Vector4 d = dot(a,a);
		Vector4 mask = notEqual(a, zero());
		Vector4 m = sqrt(d);

		return and4(mask, m);
	}

	static NX_INLINE Vector4 normalize(const Vector4 &a)
	{
		Vector4 scale = reciprocalSqrt(dot(a,a));
		return multiply(a, scale);
	}

	static NX_INLINE Vector4 normalizeSafe(const Vector4 &n, Vector4 &nLength)
	{
		nLength = magnitudeSafe(n);
		return normalizeSafe(n);
	}

	static NX_INLINE Vector4 normalizeRecip(const Vector4 &n, Vector4 &rcpLength)
	{
		rcpLength = reciprocal(magnitude(n));
		return normalize(n);
	}

	static NX_INLINE Vector4 normalizeSafe(const Vector4 &a)
	{
		Vector4 d = dot(a,a);
		Vector4 mask = notEqual(a, zero());
		
		Vector4 scale = reciprocalSqrt(d);
		Vector4 result = multiply(a, scale);

		return and4(mask, result);
	}

	static NX_INLINE Vector4 magnitudeSafe(const Vector4 &n, Vector4 &oneH)
	{
		Vector4 mag = magnitudeSafe(n);
		Vector4 mask = notEqual(mag, zero());

		oneH = and4(mask, reciprocal(mag));
		return mag;
	}

	// doesnt handle infinities, zero etc.
	static NX_INLINE void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		sq = Vector4(NxMath::sqrt(a.x), NxMath::sqrt(a.y), NxMath::sqrt(a.z), NxMath::sqrt(a.w));
		rsq = reciprocal(sq);
	}
	
	// doesnt handle infinities, zero etc.
	static NX_INLINE void sqrtAndRcpSqrtEst(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		sqrtAndRcpSqrt(a, sq, rsq);
	}

	static NX_INLINE Vector4 clamp(const Vector4 &a,const Vector4 &high, const Vector4 &low)
	{
		return Vector4(
			NxMath::clamp(a.x, high.x, low.x),
			NxMath::clamp(a.y, high.y, low.y),
			NxMath::clamp(a.z, high.z, low.z),
			NxMath::clamp(a.w, high.w, low.w));
	}

	static NX_INLINE Vector4 zero()
	{
		return Vector4(0.0f,0.0f,0.0f,0.0f);
	}

	static NX_INLINE Vector4 one()
	{
		return Vector4(1.0f,1.0f,1.0f,1.0f);
	}

	static NX_INLINE Vector4 minusOne()
	{
		return Vector4(-1.0f, -1.0f, -1.0f, -1.0f);
	}

	static NX_INLINE Vector4 half()
	{
		return Vector4(0.5f, 0.5f, 0.5f, 0.5f);
	}

	static NX_INLINE Vector4 allSet()
	{
		return Vector4(0xffFFffFF,0xffFFffFF,0xffFFffFF,0xffFFffFF);
	}

	static NX_INLINE Vector4 eps()
	{
		const static Vector4 e (PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON);
		return e;
	}

	static NX_INLINE Vector4 eps6()
	{
		const static Vector4 e (1e-6f, 1e-6f, 1e-6f, 1e-6f);
		return e;
	}

	static NX_INLINE Vector4 floatMin()
	{
		return Vector4(-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX);
	}

	static NX_INLINE Vector4 floatMax()
	{
		return Vector4(PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX);
	}

	static NX_INLINE Vector4 signMask()
	{
		return Vector4(0x80000000, 0x80000000, 0x80000000, 0x80000000);
	}

	static NX_INLINE Vector4 rotate(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiply(basis0, X);
		result = multiplyAdd(basis1, Y, result);
		result = multiplyAdd(basis2, Z, result);

		return result;
	}

	static NX_INLINE Vector4 rotateInv(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		return mergeXYZ(dot(basis0, other),
						dot(basis1, other),
						dot(basis2, other));
	}

	/* Min/max , per component */
	static NX_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? a.x : b.x,
						(a.y > b.y) ? a.y : b.y,
						(a.z > b.z) ? a.z : b.z,
						(a.w > b.w) ? a.w : b.w);
	}

	static NX_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? a.x : b.x,
						(a.y < b.y) ? a.y : b.y,
						(a.z < b.z) ? a.z : b.z,
						(a.w < b.w) ? a.w : b.w);
	}

	/* splatting */
	static NX_INLINE Vector4 splatX(const Vector4 &a)
	{
		return Vector4(a.x, a.x, a.x, a.x);
	}

	static NX_INLINE Vector4 splatY(const Vector4 &a)
	{
		return Vector4(a.y, a.y, a.y, a.y);
	}

	static NX_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return Vector4(a.z, a.z, a.z, a.z);
	}

	static NX_INLINE Vector4 splatW(const Vector4 &a)
	{
		return Vector4(a.w, a.w, a.w, a.w);
	}

	/* permutations and selects */
	static NX_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.y, vZ.z, 0);
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static NX_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.x, vZ.x, 0);
	}

	static NX_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.y, vY.y, vZ.y, 0);
	}

	static NX_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.z, vY.z, vZ.z, 0);
	}


	static NX_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		//TODO RETHINK
		Vector4 result(0.0f,0.0f,0.0f,0.0f);
		return result;
	}

	static NX_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return Vector4( (~control.ux & a.ux) | (control.ux & b.ux),
						(~control.uy & a.uy) | (control.uy & b.uy),
						(~control.uz & a.uz) | (control.uz & b.uz),
						(~control.uw & a.uw) | (control.uw & b.uw));
	}


	/* float comparisons (bitmasks)*/
	static NX_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x == b.x) ? 0xFFFFFFFF : 0,
						(a.y == b.y) ? 0xFFFFFFFF : 0,
						(a.z == b.z) ? 0xFFFFFFFF : 0,
						(a.w == b.w) ? 0xFFFFFFFF : 0);
	}

	static NX_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x != b.x) ? 0xFFFFFFFF : 0,
						(a.y != b.y) ? 0xFFFFFFFF : 0,
						(a.z != b.z) ? 0xFFFFFFFF : 0,
						(a.w != b.w) ? 0xFFFFFFFF : 0);
	}

	static NX_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? 0xFFFFFFFF : 0,
						(a.y < b.y) ? 0xFFFFFFFF : 0,
						(a.z < b.z) ? 0xFFFFFFFF : 0,
						(a.w < b.w) ? 0xFFFFFFFF : 0);
	}

	static NX_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? 0xFFFFFFFF : 0,
						(a.y > b.y) ? 0xFFFFFFFF : 0,
						(a.z > b.z) ? 0xFFFFFFFF : 0,
						(a.w > b.w) ? 0xFFFFFFFF : 0);
	}

	static NX_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x <= b.x) ? 0xFFFFFFFF : 0,
						(a.y <= b.y) ? 0xFFFFFFFF : 0,
						(a.z <= b.z) ? 0xFFFFFFFF : 0,
						(a.w <= b.w) ? 0xFFFFFFFF : 0);
	}

	static NX_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x >= b.x) ? 0xFFFFFFFF : 0,
						(a.y >= b.y) ? 0xFFFFFFFF : 0,
						(a.z >= b.z) ? 0xFFFFFFFF : 0,
						(a.w >= b.w) ? 0xFFFFFFFF : 0);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static NX_INLINE NX_BOOL equalBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	static NX_INLINE NX_BOOL notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x) && (a.y != b.y) && (a.z != b.z);
	}

	static NX_INLINE NX_BOOL lessBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
	}

	static NX_INLINE NX_BOOL greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
	}

	static NX_INLINE NX_BOOL lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
	}

	static NX_INLINE NX_BOOL greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z);
	}
	/* float comparisons, bool x component only */

	static NX_INLINE NX_BOOL equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x);
	}

	static NX_INLINE NX_BOOL notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x);
	}

	static NX_INLINE NX_BOOL lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x);
	}

	static NX_INLINE NX_BOOL greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x);
	}

	static NX_INLINE NX_BOOL lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x);
	}

	static NX_INLINE NX_BOOL greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x);
	}

	/* int comparisons (bitmasks) */
	static NX_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.ux != b.ux) ? 0xFFFFFFFF : 0,
						(a.uy != b.uy) ? 0xFFFFFFFF : 0,
						(a.uz != b.uz) ? 0xFFFFFFFF : 0,
						(a.uw != b.uw) ? 0xFFFFFFFF : 0);
	}

	/* int comparisons bool (first 3 components)*/
	static NX_INLINE NX_BOOL intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.ux != b.ux) || (a.uy != b.uy) || (a.uz != b.uz);
	}

	/* bitwise logical operations */
	static NX_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & b.ux,
						a.uy & b.uy,
						a.uz & b.uz,
						a.uw & b.uw);
	}

	static NX_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux | b.ux,
						a.uy | b.uy,
						a.uz | b.uz,
						a.uw | b.uw);
	}

	static NX_INLINE Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.ux ^ b.ux, a.uy ^ b.uy, a.uz ^ b.uz, a.uw ^ b.uw);
	}


	static NX_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & ~b.ux,
						a.uy & ~b.uy,
						a.uz & ~b.uz,
						a.uw & ~b.uw);
	}

private:
#ifdef NX_USE_PLAIN_SIMD
	NxSimd()
#else
	class NxSimdVerify()
#endif
	{}
};


#endif
