#ifndef PXC_SIMD_H
#define PXC_SIMD_H

#include "PxnError.h"
#include "PxcPlatformInclude.h"
#include "PxcVector.h"
#include "PxcPlane.h"

#if defined(PXD_PLATFORM_WINDOWS) && defined(PXD_SUPPORT_SSE)
#include "win/PxcSimd_WIN.h"
#elif defined(PXD_PLATFORM_LINUX) && defined(PXD_SUPPORT_SSE)
#include "linux/PxcSimd_LINUX.h"
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcSimd_XBOX.h"
#else
	#define PXC_USE_PLAIN_SIMD	//Define this to always force usage of C++ implementation
#endif

/*!
PxcSimd provides an abstraction of platform specific SIMD functionality. The aim is to
take advantage on SIMD on other platforms eventually, with less effort. (and aid testing)

Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. PxcVectors will be read/written as 4 component vectors!
*/

//! Plain C++ version
#ifdef PXC_USE_PLAIN_SIMD
class PxcSimd
#else
class PxcSimdVerify
#endif
{
public:
	struct Vector4
	{
		Vector4()
		{}

		Vector4(PxFloat x, PxFloat y, PxFloat z, PxFloat w)
			: x(x), y(y), z(z), w(w)
		{}

		Vector4(PxU32 x, PxU32 y, PxU32 z, PxU32 w)
			: ux(x), uy(y), uz(z), uw(w)
		{}

		union
		{
			struct 
			{
				PxFloat x, y, z, w;
			};
			struct 
			{
				PxU32 ux, uy, uz, uw;
			};
			PxFloat f[4];
			PxU32	u[4];
		};
		
	};

	struct Matrix34
	{
		Matrix34(){}
		Matrix34(const Vector4 &a, const Vector4 &b, const Vector4 &c, const Vector4 &d) : base0(a), base1(b), base2(c), base3(d){}

		Vector4 base0;
		Vector4 base1;
		Vector4 base2;
		Vector4 base3;
	};

	/* simd support */
	static PXD_FORCE_INLINE bool isSupported()
	{
		return true;
	}

	/* Setup any rounding modes etc */
	static PXD_FORCE_INLINE PxU32 setup()
	{
		return 0;
	}

	/* Reset any setup rounding modes etc*/
	static PXD_FORCE_INLINE void reset(PxU32 fromSetup)
	{
	}

	/* loads */	
	static PXD_FORCE_INLINE Vector4 load(const PxcVector &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static PXD_FORCE_INLINE Vector4 loadW0(const PxcVector &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0.0f);
	}

	static PXD_FORCE_INLINE Vector4 loadW1(const PxcVector &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 1.0f);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxcPlane &pxcPlane)
	{
		return Vector4(pxcPlane.normal.x, pxcPlane.normal.y, pxcPlane.normal.z, pxcPlane.d);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxcQuaternion &pxcQuat)
	{
		return Vector4(pxcQuat.v.x, pxcQuat.v.y, pxcQuat.v.z, pxcQuat.w);
	}

	//A = aligned
	static PXD_FORCE_INLINE Vector4 loadA(const PxcVector &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static PXD_FORCE_INLINE Vector4 loadA(const Vector4 &simdVec)
	{
		return Vector4(simdVec.x, simdVec.y, simdVec.z, simdVec.w);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxFloat &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU32 &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU8 &a)
	{
		return Vector4((PxU32)a, 0, 0, 0);
	}

	static PXD_FORCE_INLINE Vector4 load4(const PxFloat* a)
	{
		return Vector4(a[0], a[1], a[2], a[3]);
	}

	static PXD_FORCE_INLINE Matrix34 load(const PxcMatrix34 &a)
	{
		return Matrix34(loadW0(a.base0), loadW0(a.base1), loadW0(a.base2), loadW1(a.base3));
	}

	/* stores */
	static PXD_FORCE_INLINE void store(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

	static PXD_FORCE_INLINE void store(PxcQuaternion &pxcQuat, const Vector4 &simdQuat)
	{
		pxcQuat.v.x = simdQuat.x;
		pxcQuat.v.y = simdQuat.y;
		pxcQuat.v.z = simdQuat.z;
		pxcQuat.w = simdQuat.w;
	}

	//A = aligned
	static PXD_FORCE_INLINE void storeA(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

		//WARNING: Use with care as this writes to the dword after pxcVec.
	static PXD_FORCE_INLINE void store4A(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
		//Match SIMD version.
		PxFloat *flt = (PxFloat *)&pxcVec;
		flt[3] = simdVec.w;
	}
	
	static PXD_FORCE_INLINE void storeElementA(PxFloat &a, const Vector4 &simdVec)
	{
		PxU8 *sElem = (PxU8 *)&a;
		PXN_ASSERT(( ((size_t)sElem) % 4) == 0);
		PxU8 *aElem = sElem - ( ((size_t)sElem) % 16);

		size_t idx = sElem - aElem;

		switch(idx)
		{
		case 0:
			a = simdVec.x;
			break;

		case 4:
			a = simdVec.y;
			break;

		case 8:
			a = simdVec.z;
			break;

		case 12:
			a = simdVec.w;
			break;

		default:
			PXN_ASSERT(!"Invalid element offset");

		}

	}

	static PXD_FORCE_INLINE void store(PxFloat &a, const Vector4 &simdVec)
	{
		a = simdVec.x;
	}

	static PXD_FORCE_INLINE void store(PxU32 &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}

	static PXD_FORCE_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}

	static PXD_FORCE_INLINE void unpack3to4(const Vector4 &inA, const Vector4 &inB, const Vector4 &inC,
		Vector4 &outA, Vector4 &outB, Vector4 &outC, Vector4 &outD)
	{
		//we have 4, 3 component vectors packed into 4 vector4s.

		outA = Vector4(inA.x, inA.y, inA.z, inA.w);
		outB = Vector4(inA.w, inB.x, inB.y, inB.z);
		outC = Vector4(inB.z, inB.w, inC.x, inC.y);
		outD = Vector4(inC.y, inC.z, inC.w, inC.x);
	}

	/* basic math */
	static PXD_FORCE_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
	}

	static PXD_FORCE_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
	}

	static PXD_FORCE_INLINE Vector4 negate(const Vector4 &a)
	{
		return Vector4(-a.x, -a.y, -a.z, -a.w);
	}

	static PXD_FORCE_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
	}

	static PXD_FORCE_INLINE Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return add(multiply(a, b), c);
	}

	static PXD_FORCE_INLINE Vector4 negMultiplySubtract(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return negate(subtract(multiply(a, b), c));
	}

	static PXD_FORCE_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return Vector4(1.0f/a.x, 1.0f/a.y, 1.0f/a.z, 1.0f/a.w);
	}

	static PXD_FORCE_INLINE Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return reciprocal(Vector4(PxcSqrt(a.x),PxcSqrt(a.y),PxcSqrt(a.z),PxcSqrt(a.w)));
	}

	// 3 component dot product
	static PXD_FORCE_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		PxFloat d = a.x*b.x + a.y*b.y + a.z*b.z;
		return Vector4(d,d,d,d);
	}

	static PXD_FORCE_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		PxFloat d = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
		return Vector4(d,d,d,d);
	}

	static PXD_FORCE_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.y*b.z - a.z*b.y,
						a.z*b.x - a.x*b.z,
						a.x*b.y - a.y*b.x,
						0);
	}

	static PXD_FORCE_INLINE Vector4 abs(const Vector4 &a)
	{
		return Vector4(PxcAbs(a.x), PxcAbs(a.y), PxcAbs(a.z), PxcAbs(a.w));
	}

	static PXD_FORCE_INLINE Vector4 zero()
	{
		return Vector4(0.0f,0.0f,0.0f,0.0f);
	}

	static PXD_FORCE_INLINE Vector4 one()
	{
		return Vector4(1.0f,1.0f,1.0f,1.0f);
	}

	static PXD_FORCE_INLINE Vector4 two()
	{
		return Vector4(2.0f, 2.0f, 2.0f, 2.0f);
	}

	static PXD_FORCE_INLINE Vector4 minusOne()
	{
		return Vector4(-1.0f, -1.0f, -1.0f, -1.0f);
	}

	static PXD_FORCE_INLINE Vector4 allSet()
	{
		return Vector4((PxU32)0xffFFffFF,(PxU32)0xffFFffFF,(PxU32)0xffFFffFF,(PxU32)0xffFFffFF);
	}

	static PXD_FORCE_INLINE Vector4 xMask()
	{
		return Vector4((PxU32)0xffFFffFF, (PxU32)0, (PxU32)0, (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 yMask()
	{
		return Vector4((PxU32)0, (PxU32)0xffFFffFF, (PxU32)0, (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 zMask()
	{
		return Vector4((PxU32)0, (PxU32)0, (PxU32)0xffFFffFF, (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 wMask()
	{
		return Vector4((PxU32)0, (PxU32)0, (PxU32)0, (PxU32)0xffFFffFF);
	}

	static PXD_FORCE_INLINE Vector4 eps()
	{
		const static Vector4 e (PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON);
		return e;
	}

	static PXD_FORCE_INLINE Vector4 eps6()
	{
		const static Vector4 e (1e-6f, 1e-6f, 1e-6f, 1e-6f);
		return e;
	}

	static PXD_FORCE_INLINE Vector4 floatMin()
	{
		return Vector4(-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX);
	}

	static PXD_FORCE_INLINE Vector4 floatMax()
	{
		return Vector4(PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX);
	}

	static PXD_FORCE_INLINE Vector4 signMask()
	{
		return Vector4((PxU32)0x80000000, (PxU32)0x80000000, (PxU32)0x80000000, (PxU32)0x80000000);
	}

	static PXD_FORCE_INLINE Vector4 rotate(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiply(basis0, X);
		result = multiplyAdd(basis1, Y, result);
		result = multiplyAdd(basis2, Z, result);

		return result;
	}

	static PXD_FORCE_INLINE Vector4 rotateInv(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		return mergeXYZ(dot(basis0, other),
						dot(basis1, other),
						dot(basis2, other));
	}

	static PXD_FORCE_INLINE Vector4 rotateQuat(const Vector4 &quat, const Vector4 &vec)
	{
		//return src * (2.0f*w*w-1.0f) + v.cross(src)*w*2.0f + v*v.dot(src)*2.0f;

		//hmmm maybe the XM version is faster? (doesnt look it though... and less portable)
		Vector4 allW = splatW(quat);
		Vector4 twoW = add(allW, allW);
		Vector4 twoWW = multiply(twoW, allW);

		Vector4 vCrossVec = cross(quat, vec);
		Vector4 vvDotVec = multiply(quat, dot(quat, vec));

		Vector4 res = multiply(vec, subtract(twoWW, one()));
		res = multiplyAdd(vCrossVec, twoW, res);
		res = multiplyAdd(vvDotVec, two(), res);
		return res;
	}

	static PXD_FORCE_INLINE Vector4 multiplyQuat(const Vector4 &a, const Vector4 &b)
	{
				/*return PxcQuaternion(v.cross(other.v) + other.v*w + v*other.w, 
							w*other.w - v.dot(other.v));*/


		Vector4 aAllW = splatW(a);
		Vector4 bAllW = splatW(b);

		Vector4 aCrossB = cross(a, b);
		Vector4 aDotB = dot(a, b);

		Vector4 res = multiplyAdd(b, aAllW, aCrossB);
		res = multiplyAdd(a, bAllW, res);

		Vector4 resW = subtract(multiply(aAllW, bAllW), aDotB);

		return select(res, resW, wMask());
	}

	static PXD_FORCE_INLINE Vector4 insertW(const Vector4 &a, const Vector4 &insMe)
	{
		return Vector4(a.x, a.y, a.z, insMe.w);
	}

	static PXD_FORCE_INLINE void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		sq = Vector4(PxcSqrt(a.x), PxcSqrt(a.y), PxcSqrt(a.z), PxcSqrt(a.w));
		rsq = reciprocal(sq);
	}

	/* Min/max , per component */
	static PXD_FORCE_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? a.x : b.x,
						(a.y > b.y) ? a.y : b.y,
						(a.z > b.z) ? a.z : b.z,
						(a.w > b.w) ? a.w : b.w);
	}

	static PXD_FORCE_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? a.x : b.x,
						(a.y < b.y) ? a.y : b.y,
						(a.z < b.z) ? a.z : b.z,
						(a.w < b.w) ? a.w : b.w);
	}

	static PXD_FORCE_INLINE Vector4 clamp(const Vector4 &a,const Vector4 &high, const Vector4 &low)
	{
		return Vector4(
			PxcClamp(a.x, high.x, low.x),
			PxcClamp(a.y, high.y, low.y),
			PxcClamp(a.z, high.z, low.z),
			PxcClamp(a.w, high.w, low.w));
	}

	/* splatting */
	static PXD_FORCE_INLINE Vector4 splatX(const Vector4 &a)
	{
		return Vector4(a.x, a.x, a.x, a.x);
	}

	static PXD_FORCE_INLINE Vector4 splatY(const Vector4 &a)
	{
		return Vector4(a.y, a.y, a.y, a.y);
	}

	static PXD_FORCE_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return Vector4(a.z, a.z, a.z, a.z);
	}

	static PXD_FORCE_INLINE Vector4 splatW(const Vector4 &a)
	{
		return Vector4(a.w, a.w, a.w, a.w);
	}

	/* permutations and selects */
	static PXD_FORCE_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.y, vZ.z, 0);
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static PXD_FORCE_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.x, vZ.x, 0);
	}

	static PXD_FORCE_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.y, vY.y, vZ.y, 0);
	}

	static PXD_FORCE_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.z, vY.z, vZ.z, 0);
	}


	static PXD_FORCE_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		Vector4 result(0.0f,0.0f,0.0f,0.0f);

		if(e0 < 4)
			result.u[0] = a.u[e0];
		else
			result.u[0] = b.u[e0 - 4];

		if(e1 < 4)
			result.u[1] = a.u[e1];
		else
			result.u[1] = b.u[e1 - 4];

		if(e2 < 4)
			result.u[2] = a.u[e2];
		else
			result.u[2] = b.u[e2 - 4];

		if(e3 < 4)
			result.u[3] = a.u[e3];
		else
			result.u[3] = b.u[e3 - 4];

		return result;
	}

	static PXD_FORCE_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return Vector4( (~control.ux & a.ux) | (control.ux & b.ux),
						(~control.uy & a.uy) | (control.uy & b.uy),
						(~control.uz & a.uz) | (control.uz & b.uz),
						(~control.uw & a.uw) | (control.uw & b.uw));
	}


	/* float comparisons (bitmasks)*/
	static PXD_FORCE_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x == b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y == b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z == b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w == b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x != b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y != b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z != b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w != b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y < b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z < b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w < b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y > b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z > b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w > b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x <= b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y <= b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z <= b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w <= b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	static PXD_FORCE_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x >= b.x) ? 0xFFFFFFFF : (PxU32)0,
						(a.y >= b.y) ? 0xFFFFFFFF : (PxU32)0,
						(a.z >= b.z) ? 0xFFFFFFFF : (PxU32)0,
						(a.w >= b.w) ? 0xFFFFFFFF : (PxU32)0);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static PXD_FORCE_INLINE PxIBool equalBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	static PXD_FORCE_INLINE PxIBool notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x) && (a.y != b.y) && (a.z != b.z);
	}

	static PXD_FORCE_INLINE PxIBool lessBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
	}

	static PXD_FORCE_INLINE PxIBool greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
	}

	static PXD_FORCE_INLINE PxIBool lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z);
	}
	/* float comparisons, bool x component only */

	static PXD_FORCE_INLINE PxIBool equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x);
	}

	static PXD_FORCE_INLINE PxIBool notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x);
	}

	static PXD_FORCE_INLINE PxIBool lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x);
	}

	static PXD_FORCE_INLINE PxIBool greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x);
	}

	static PXD_FORCE_INLINE PxIBool lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x);
	}

	/* int comparisons (bitmasks) */
	static PXD_FORCE_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.ux != b.ux) ? 0xFFFFFFFF : (PxU32)0,
						(a.uy != b.uy) ? 0xFFFFFFFF : (PxU32)0,
						(a.uz != b.uz) ? 0xFFFFFFFF : (PxU32)0,
						(a.uw != b.uw) ? 0xFFFFFFFF : (PxU32)0);
	}

	/* int comparisons bool (first 3 components)*/
	static PXD_FORCE_INLINE PxIBool intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.ux != b.ux) || (a.uy != b.uy) || (a.uz != b.uz);
	}

	/* bitwise logical operations */
	static PXD_FORCE_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & b.ux,
						a.uy & b.uy,
						a.uz & b.uz,
						a.uw & b.uw);
	}

	static PXD_FORCE_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux | b.ux,
						a.uy | b.uy,
						a.uz | b.uz,
						a.uw | b.uw);
	}

	static PXD_FORCE_INLINE Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.ux ^ b.ux, a.uy ^ b.uy, a.uz ^ b.uz, a.uw ^ b.uw);
	}

	static PXD_FORCE_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & ~b.ux,
						a.uy & ~b.uy,
						a.uz & ~b.uz,
						a.uw & ~b.uw);
	}

private:
#ifdef PXC_USE_PLAIN_SIMD
	PxcSimd()
#else
	class PxcSimdVerify()
#endif
	{}
};


#endif
