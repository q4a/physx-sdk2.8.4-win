#ifndef OPC_SIMD_H
#define OPC_SIMD_H


/*
This is the opcode version of PxcSimd, it is painful having PxcSimd, NxSimd and OpcSimd but it is duplicated so
we dont introduce dependancies. PxcSimd is considered the master file and OpcSimd/NxSimd are just versions altered to
conform to the respective library.
*/

/*!
OpcSimd provides an abstraction of platform specific SIMD functionality. The aim is to
take advantage on SIMD on other platforms eventually, with less effort. (and aid testing)

Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. Points will be read/written as 4 component vectors!
*/

//! Use SIMD on xbox360
#if defined(_XBOX)
#include "xbox360/OPC_Simd_XBOX.h"
#else
#define OPC_USE_PLAIN_SIMD
#endif


//! Plain C++ version
#ifdef OPC_USE_PLAIN_SIMD

#define OPC_SIMD_0X 0
#define OPC_SIMD_0Y 1
#define OPC_SIMD_0Z 2
#define OPC_SIMD_0W 3
#define OPC_SIMD_1X 4
#define OPC_SIMD_1Y 5
#define OPC_SIMD_1Z 6
#define OPC_SIMD_1W 7


#define OPC_SIMD_PERMUTE(name, a, b, c, d) static const OpcSimd::Vector4 name((udword)(a), (udword)(b), (udword)(c), (udword)(d))


class OpcSimd
#else

#define OPC_SIMD_VERIFY_0X 0
#define OPC_SIMD_VERIFY_0Y 1
#define OPC_SIMD_VERIFY_0Z 2
#define OPC_SIMD_VERIFY_0W 3
#define OPC_SIMD_VERIFY_1X 4
#define OPC_SIMD_VERIFY_1Y 5
#define OPC_SIMD_VERIFY_1Z 6
#define OPC_SIMD_VERIFY_1W 7


#define OPC_SIMD_VERIFY_PERMUTE(name, a, b, c, d) static const OpcSimdVerify::Vector4 name((udword)(a),(udword(b),(udword)(c),(udword)(d))


class OpcSimdVerify
#endif
{
public:
	struct Vector4
	{
		Vector4()
		{}

		Vector4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w)
		{}

		Vector4(udword x, udword y, udword z, udword w)
			: ux(x), uy(y), uz(z), uw(w)
		{}

		union
		{
			struct 
			{
				float x, y, z, w;
			};
			struct 
			{
				udword ux, uy, uz, uw;
			};
			float f[4];
			udword	u[4];
		};
		
	};

	typedef Vector4 Vector4I;
	/* simd support */
	static inline_ bool isSupported()
	{
		return true;
	}

	/* Setup any rounding modes etc */
	static inline_ udword setup()
	{
		return 0;
	}

	/* Reset any setup rounding modes etc*/
	static inline_ void reset(udword fromSetup)
	{
	}

	/* loads */	
	static inline_ Vector4 load(const Point &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static inline_ Vector4 loadW1(const Point &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 1.0f);
	}

	static inline_ Vector4 load(const Plane &Plane)
	{
		return Vector4(Plane.n.x, Plane.n.y, Plane.n.z, Plane.d);
	}

	/*static inline_ Vector4 load(const PxcQuaternion &pxcQuat)
	{
		return Vector4(pxcQuat.x, pxcQuat.y, pxcQuat.z, pxcQuat.w);
	}*/

	//A = aligned
	static inline_ Vector4 loadA(const Point &pxcVec)
	{
		return Vector4(pxcVec.x, pxcVec.y, pxcVec.z, 0);
	}

	static inline_ Vector4 load(const float &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	static inline_ Vector4 load(const udword &a)
	{
		return Vector4(a, 0, 0, 0);
	}

	/*static inline_ Vector4 load(const PxU8 &a)
	{
		return Vector4((udword)a, 0, 0, 0);
	}*/

	static inline_ Vector4 load4(const float* a)
	{
		return Vector4(a[0], a[1], a[2], a[3]);
	}

	static inline_ Vector4 load(const Vector4 &a)
	{
		return a;
	}

	static inline_ Vector4 loadToFloat3(const sword *a)
	{
		return Vector4((float)a[0], (float)a[1], (float)a[2], 0);
	}

	static inline_ Vector4 loadToFloat3(const uword *a)
	{
		return Vector4((float)a[0], (float)a[1], (float)a[2], 0);
	}

	/* stores */
	static inline_ void store(Point &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

/*	static inline_ void store(PxcQuaternion &pxcQuat, const Vector4 &simdQuat)
	{
		pxcQuat.x = simdQuat.x;
		pxcQuat.y = simdQuat.y;
		pxcQuat.z = simdQuat.z;
		pxcQuat.w = simdQuat.w;
	}*/

	//A = aligned
	static inline_ void storeA(Point &pxcVec, const Vector4 &simdVec)
	{
		pxcVec.x = simdVec.x;
		pxcVec.y = simdVec.y;
		pxcVec.z = simdVec.z;
	}

	static inline_ void store(float &a, const Vector4 &simdVec)
	{
		a = simdVec.x;
	}

	static inline_ void store(udword &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}

	/*static inline_ void store(PxU8 &a, const Vector4 &simdVec)
	{
		a = simdVec.ux;
	}*/

	/* basic math */
	static inline_ Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
	}

	static inline_ Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
	}

	static inline_ Vector4 negate(const Vector4 &a)
	{
		return Vector4(-a.x, -a.y, -a.z, -a.w);
	}

	static inline_ Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
	}

	static inline_ Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return add(multiply(a, b), c);
	}

	static inline_ Vector4 reciprocal(const Vector4 &a)
	{
		return Vector4(1.0f/a.x, 1.0f/a.y, 1.0f/a.z, 1.0f/a.w);
	}

	static inline_ Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return reciprocal(Vector4(sqrtf(a.x),sqrtf(a.y),sqrtf(a.z),sqrtf(a.w)));
	}

	// 3 component dot product
	static inline_ Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		float d = a.x*b.x + a.y*b.y + a.z*b.z;
		return Vector4(d,d,d,d);
	}

	static inline_ Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		float d = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
		return Vector4(d,d,d,d);
	}

	static inline_ Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.y*b.z - a.z*b.y,
						a.z*b.x - a.x*b.z,
						a.x*b.y - a.y*b.x,
						0);
	}

	static inline_ Vector4 abs(const Vector4 &a)
	{
		return Vector4(fabsf(a.x), fabsf(a.y), fabsf(a.z), fabsf(a.w));
	}

	static inline_ Vector4 zero()
	{
		return Vector4(0.0f,0.0f,0.0f,0.0f);
	}

	static inline_ Vector4 one()
	{
		return Vector4(1.0f,1.0f,1.0f,1.0f);
	}

	static inline_ Vector4 half()
	{
		return Vector4(0.5f, 0.5f, 0.5f, 0.5f);
	}

	static inline_ Vector4 two()
	{
		return Vector4(2.0f, 2.0f, 2.0f, 2.0f);
	}

	static inline_ Vector4 minusOne()
	{
		return Vector4(-1.0f, -1.0f, -1.0f, -1.0f);
	}

	static inline_ Vector4 allSet()
	{
		return Vector4(0xffFFffFF,0xffFFffFF,0xffFFffFF,0xffFFffFF);
	}

	static inline_ Vector4 xMask()
	{
		return Vector4(0xffFFffFF, 0, 0, 0);
	}

	static inline_ Vector4 yMask()
	{
		return Vector4(0, 0xffFFffFF, 0, 0);
	}

	static inline_ Vector4 zMask()
	{
		return Vector4(0, 0, 0xffFFffFF, 0);
	}

	static inline_ Vector4 wMask()
	{
		return Vector4(0, 0, 0, 0xffFFffFF);
	}

	static inline_ Vector4 xyzMask()
	{
		return Vector4(0xffFFffFF, 0xffFFffFF, 0xffFFffFF, 0);
	}

	static inline_ Vector4 signMask()
	{
		return Vector4(0x80000000, 0x80000000, 0x80000000, 0x80000000);
	}

	/*static inline_ Vector4 eps()
	{
		const static Vector4 e (OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON);
		return e;
	}*/

	static inline_ Vector4 eps6()
	{
		const static Vector4 e (1e-6f, 1e-6f, 1e-6f, 1e-6f);
		return e;
	}

/*	static inline_ Vector4 floatMin()
	{
		return Vector4(-OPC_FLOAT_MAX, -OPC_FLOAT_MAX, -OPC_FLOAT_MAX, -OPC_FLOAT_MAX);
	}

	static inline_ Vector4 floatMax()
	{
		return Vector4(OPC_FLOAT_MAX, OPC_FLOAT_MAX, OPC_FLOAT_MAX, OPC_FLOAT_MAX);
	}*/

	static inline_ Vector4 rotate(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiply(basis0, X);
		result = multiplyAdd(basis1, Y, result);
		result = multiplyAdd(basis2, Z, result);

		return result;
	}
	static inline_ Vector4 transform(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &trans, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiplyAdd(basis0, X, trans);
		result = multiplyAdd(basis1, Y, result);
		result = multiplyAdd(basis2, Z, result);

		return result;
	}
	static inline_ Vector4 rotateInv(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		return mergeXYZ(dot(basis0, other),
						dot(basis1, other),
						dot(basis2, other));
	}

	static inline_ Vector4 rotateQuat(const Vector4 &quat, const Vector4 &vec)
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

	static inline_ Vector4 multiplyQuat(const Vector4 &a, const Vector4 &b)
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


	static inline_ bool pointOnPlaneInTriangle(const Vector4 &pt, 
		const Vector4 &v0, const Vector4 &v1, const Vector4 &v2)
	{
		Vector4 normal = cross(subtract(v2, v0), subtract(v1, v0));

		// cross products with edges

		Vector4 a = cross(subtract(pt, v0), subtract(v1, v0));
		Vector4 b = cross(subtract(pt, v1), subtract(v2, v1));
		Vector4 c = cross(subtract(pt, v2), subtract(v0, v2));

		Vector4 z = zero();

		// cross products point in same dir as normal if the point is inside

		return
			greaterEqualXBool(dot(a, normal), z) && //should we do this with masks?
			greaterEqualXBool(dot(b, normal), z) &&
			greaterEqualXBool(dot(c, normal), z);
	}

	static Vector4 pointOnLineSegNearPoint(const Vector4 &v0, const Vector4 &v1, const Vector4 &pt)
	{
		Vector4 dir = subtract(v1, v0);
		Vector4 proj = subtract(dot(pt, dir), dot(v0, dir));
		Vector4 lenSq = dot(dir, dir);

		Vector4 t = multiply(proj, reciprocal(lenSq));
		Vector4 rv = add(v0, multiply(t, dir));

		// clamp to seg

		Vector4 lessMask = less(proj, zero());
		Vector4 greatMask = greater(proj, lenSq);

		rv = select(rv, v0, lessMask);
		rv = select(rv, v1, greatMask);

		return rv;
	}

	static inline_ void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		sq = Vector4(sqrtf(a.x), sqrtf(a.y), sqrtf(a.z), sqrtf(a.w));
		rsq = reciprocal(sq);
	}

	static inline_ Vector4 sqrt(const Vector4 &a)
	{
		return Vector4(sqrtf(a.x), sqrtf(a.y), sqrtf(a.z), sqrtf(a.w));
	}

	/* Min/max , per component */
	static inline_ Vector4 max4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? a.x : b.x,
						(a.y > b.y) ? a.y : b.y,
						(a.z > b.z) ? a.z : b.z,
						(a.w > b.w) ? a.w : b.w);
	}

	static inline_ Vector4 min4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? a.x : b.x,
						(a.y < b.y) ? a.y : b.y,
						(a.z < b.z) ? a.z : b.z,
						(a.w < b.w) ? a.w : b.w);
	}

	static inline_ Vector4 clamp(const Vector4 &a,const Vector4 &high, const Vector4 &low)
	{
		return min4(high, max4(a, low));
	}

	/* splatting */
	static inline_ Vector4 splatX(const Vector4 &a)
	{
		return Vector4(a.x, a.x, a.x, a.x);
	}

	static inline_ Vector4 splatY(const Vector4 &a)
	{
		return Vector4(a.y, a.y, a.y, a.y);
	}

	static inline_ Vector4 splatZ(const Vector4 &a)
	{
		return Vector4(a.z, a.z, a.z, a.z);
	}

	static inline_ Vector4 splatW(const Vector4 &a)
	{
		return Vector4(a.w, a.w, a.w, a.w);
	}

	/* permutations and selects */
	static inline_ Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.y, vZ.z, 0);
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static inline_ Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.x, vY.x, vZ.x, 0);
	}

	static inline_ Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.y, vY.y, vZ.y, 0);
	}

	static inline_ Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		return Vector4(vX.z, vY.z, vZ.z, 0);
	}

	static inline_ void transpose3x3(const Vector4 &inA, const Vector4 &inB, const Vector4 &inC, 
		Vector4 &outA, Vector4 &outB, Vector4 &outC)
	{
		outA.x = inA.x; outA.y = inB.x; outA.z = inC.x;
		outB.x = inA.y; outB.y = inB.y; outB.z = inC.y;
		outC.x = inA.z; outC.y = inB.z; outC.z = inC.z;
	}

	static inline_ Vector4 insertW(const Vector4 &a, const Vector4 &insMe)
	{
		return Vector4(a.x, a.y, a.z, insMe.w);
	}

	static inline_ Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const udword e0,const udword e1,const udword e2,const udword e3)
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

	static inline_ Vector4 permute(const Vector4 &a, const Vector4 &b, const Vector4 &ctrl)
	{
		return permute(a, b, ctrl.ux, ctrl.uy, ctrl.uz, ctrl.uw);
	}

	static inline_ Vector4 permuteXXX(const Vector4 &a)	{ return Vector4(a.x, a.x, a.x, a.w); }
	static inline_ Vector4 permuteXXY(const Vector4 &a) { return Vector4(a.x, a.x, a.y, a.w); }
	static inline_ Vector4 permuteXXZ(const Vector4 &a)	{ return Vector4(a.x, a.x, a.z, a.w); }

	static inline_ Vector4 permuteXYX(const Vector4 &a)	{ return Vector4(a.x, a.y, a.x, a.w); }
	static inline_ Vector4 permuteXYY(const Vector4 &a) { return Vector4(a.x, a.y, a.y, a.w); }
	static inline_ Vector4 permuteXYZ(const Vector4 &a)	{ return Vector4(a.x, a.y, a.z, a.w); }

	static inline_ Vector4 permuteXZX(const Vector4 &a)	{ return Vector4(a.x, a.z, a.x, a.w); }
	static inline_ Vector4 permuteXZY(const Vector4 &a) { return Vector4(a.x, a.z, a.y, a.w); }
	static inline_ Vector4 permuteXZZ(const Vector4 &a)	{ return Vector4(a.x, a.z, a.z, a.w); }



	static inline_ Vector4 permuteYXX(const Vector4 &a)	{ return Vector4(a.y, a.x, a.x, a.w); }
	static inline_ Vector4 permuteYXY(const Vector4 &a) { return Vector4(a.y, a.x, a.y, a.w); }
	static inline_ Vector4 permuteyXZ(const Vector4 &a)	{ return Vector4(a.y, a.x, a.z, a.w); }

	static inline_ Vector4 permuteYYX(const Vector4 &a)	{ return Vector4(a.y, a.y, a.x, a.w); }
	static inline_ Vector4 permuteYYY(const Vector4 &a) { return Vector4(a.y, a.y, a.y, a.w); }
	static inline_ Vector4 permuteYYZ(const Vector4 &a)	{ return Vector4(a.y, a.y, a.z, a.w); }

	static inline_ Vector4 permuteYZX(const Vector4 &a)	{ return Vector4(a.y, a.z, a.x, a.w); }
	static inline_ Vector4 permuteYZY(const Vector4 &a) { return Vector4(a.y, a.z, a.y, a.w); }
	static inline_ Vector4 permuteYZZ(const Vector4 &a)	{ return Vector4(a.y, a.z, a.z, a.w); }

	
	
	static inline_ Vector4 permuteZXX(const Vector4 &a)	{ return Vector4(a.z, a.x, a.x, a.w); }
	static inline_ Vector4 permuteZXY(const Vector4 &a) { return Vector4(a.z, a.x, a.y, a.w); }
	static inline_ Vector4 permuteZXZ(const Vector4 &a)	{ return Vector4(a.z, a.x, a.z, a.w); }

	static inline_ Vector4 permuteZYX(const Vector4 &a)	{ return Vector4(a.z, a.y, a.x, a.w); }
	static inline_ Vector4 permuteZYY(const Vector4 &a) { return Vector4(a.z, a.y, a.y, a.w); }
	static inline_ Vector4 permuteZYZ(const Vector4 &a)	{ return Vector4(a.z, a.y, a.z, a.w); }

	static inline_ Vector4 permuteZZX(const Vector4 &a)	{ return Vector4(a.z, a.z, a.x, a.w); }
	static inline_ Vector4 permuteZZY(const Vector4 &a) { return Vector4(a.z, a.z, a.y, a.w); }
	static inline_ Vector4 permuteZZZ(const Vector4 &a)	{ return Vector4(a.z, a.z, a.z, a.w); }

	static inline_ Vector4 permuteXYZX(const Vector4 &a) { return Vector4(a.x, a.y, a.z, a.x); }

	static inline_ Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return Vector4( (~control.ux & a.ux) | (control.ux & b.ux),
						(~control.uy & a.uy) | (control.uy & b.uy),
						(~control.uz & a.uz) | (control.uz & b.uz),
						(~control.uw & a.uw) | (control.uw & b.uw));
	}


	/* float comparisons (bitmasks)*/
	static inline_ Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x == b.x) ? 0xFFFFFFFF : 0,
						(a.y == b.y) ? 0xFFFFFFFF : 0,
						(a.z == b.z) ? 0xFFFFFFFF : 0,
						(a.w == b.w) ? 0xFFFFFFFF : 0);
	}

	static inline_ Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x != b.x) ? 0xFFFFFFFF : 0,
						(a.y != b.y) ? 0xFFFFFFFF : 0,
						(a.z != b.z) ? 0xFFFFFFFF : 0,
						(a.w != b.w) ? 0xFFFFFFFF : 0);
	}

	static inline_ Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x < b.x) ? 0xFFFFFFFF : 0,
						(a.y < b.y) ? 0xFFFFFFFF : 0,
						(a.z < b.z) ? 0xFFFFFFFF : 0,
						(a.w < b.w) ? 0xFFFFFFFF : 0);
	}

	static inline_ Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x > b.x) ? 0xFFFFFFFF : 0,
						(a.y > b.y) ? 0xFFFFFFFF : 0,
						(a.z > b.z) ? 0xFFFFFFFF : 0,
						(a.w > b.w) ? 0xFFFFFFFF : 0);
	}

	static inline_ Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x <= b.x) ? 0xFFFFFFFF : 0,
						(a.y <= b.y) ? 0xFFFFFFFF : 0,
						(a.z <= b.z) ? 0xFFFFFFFF : 0,
						(a.w <= b.w) ? 0xFFFFFFFF : 0);
	}

	static inline_ Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.x >= b.x) ? 0xFFFFFFFF : 0,
						(a.y >= b.y) ? 0xFFFFFFFF : 0,
						(a.z >= b.z) ? 0xFFFFFFFF : 0,
						(a.w >= b.w) ? 0xFFFFFFFF : 0);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static inline_ bool equalBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	static inline_ bool notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x) && (a.y != b.y) && (a.z != b.z);
	}

	static inline_ bool lessBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
	}

	static inline_ bool greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
	}

	static inline_ bool lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
	}

	static inline_ bool greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z);
	}
	/* float comparisons, bool x component only */

	static inline_ bool equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x == b.x);
	}

	static inline_ bool notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x != b.x);
	}

	static inline_ bool lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x < b.x);
	}

	static inline_ bool greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x > b.x);
	}

	static inline_ bool lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x);
	}

	static inline_ bool greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x);
	}

	static inline_ bool greaterEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z) && (a.w >= b.w);
	}

	static inline_ bool lessEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z) && (a.w <= b.w);
	}

	static inline_ bool inBounds4Bool(const Vector4 &a, const Vector4 &bounds)
	{
		return (
			(a.x <= bounds.x && a.x >= -bounds.x) && 
			(a.y <= bounds.y && a.y >= -bounds.y) && 
			(a.z <= bounds.z && a.z >= -bounds.z) && 
			(a.w <= bounds.w && a.w >= -bounds.w));
	}

	/* int comparisons (bitmasks) */
	static inline_ Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( (a.ux != b.ux) ? 0xFFFFFFFF : 0,
						(a.uy != b.uy) ? 0xFFFFFFFF : 0,
						(a.uz != b.uz) ? 0xFFFFFFFF : 0,
						(a.uw != b.uw) ? 0xFFFFFFFF : 0);
	}

	/* int comparisons bool (first 3 components)*/
	static inline_ bool intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return (a.ux != b.ux) || (a.uy != b.uy) || (a.uz != b.uz);
	}

	static inline_ bool intNotEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return (a.ux != b.ux) || (a.uy != b.uy) || (a.uz != b.uz) || (a.uw != b.uw);
	}


	/* bitwise logical operations */
	static inline_ Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & b.ux,
						a.uy & b.uy,
						a.uz & b.uz,
						a.uw & b.uw);
	}

	static inline_ Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux | b.ux,
						a.uy | b.uy,
						a.uz | b.uz,
						a.uw | b.uw);
	}

	static inline_ Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return Vector4(a.ux ^ b.ux, a.uy ^ b.uy, a.uz ^ b.uz, a.uw ^ b.uw);
	}

	static inline_ Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return Vector4( a.ux & ~b.ux,
						a.uy & ~b.uy,
						a.uz & ~b.uz,
						a.uw & ~b.uw);
	}

private:
#ifdef OPC_USE_PLAIN_SIMD
	OpcSimd()
#else
	class OpcSimdVerify()
#endif
	{}
};


#endif
