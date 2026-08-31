#ifndef PXC_SIMD_XBOX_H
#define PXC_SIMD_XBOX_H

#include "PxnError.h"

#include "PxcPlatformInclude.h"
#include "PxcVector.h"
#include "PxcPlane.h"

/*!
Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. PxcVectors will be read/written as 4 component vectors!
*/

class PxcSimd
{
public:
	typedef XMVECTOR Vector4;

	struct Matrix34
	{
		Matrix34(){}
		Matrix34(Vector4 &a, Vector4 &b, Vector4 &c, Vector4 &d) : base0(a), base1(b), base2(c), base3(d){}

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
		//use this unsafve version?
		return XMLoadVector4(&pxcVec);
		//return XMLoadVector3(&pxcVec);
	}

	static PXD_FORCE_INLINE Vector4 load4(const PxcVector &pxcVec)
	{	
		//load {vec3, float} at once
		return XMLoadVector4A(&pxcVec);		
	}

	static PXD_FORCE_INLINE Vector4 load4(const PxFloat &a)
	{	
		//load 4 float at once
		return XMLoadVector4A(&a);		
	}	

	static PXD_FORCE_INLINE Vector4 loadW0(const PxcVector &pxcVec)
	{
		XMVECTOR rv = XMLoadVector3(&pxcVec);
		rv = insertW(rv, zero());
		return rv;
	}

	static PXD_FORCE_INLINE Vector4 loadW1(const PxcVector &pxcVec)
	{
		XMVECTOR rv = XMLoadVector3(&pxcVec);
		rv = insertW(rv, one());
		return rv;
	}

	static PXD_FORCE_INLINE Vector4 loadA(const PxcVector &pxcVec)
	{
		return XMLoadVector3A(&pxcVec);
	}

	static PXD_FORCE_INLINE Vector4 loadA(const Vector4 &simdVec)
	{
		return XMLoadVector4A(&simdVec);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxcPlane &pxcPlane)
	{
		return XMLoadVector4(&pxcPlane);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxcQuaternion &pxcQuat)
	{
		return XMLoadVector4(&pxcQuat);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxFloat &a)
	{
		return XMLoadScalar(&a);
	}

	static PXD_FORCE_INLINE Vector4 loadSplat(const PxFloat &a)
	{
		return XMVectorSplatX(XMLoadScalar(&a));
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU32 &a)
	{
		return XMLoadScalar(&a);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU8 &a)
	{
		__vector4 tmp = __lvlx(&a, 0); // load to left of register
		__vector4 rShift = __vspltisw(-8); //shift by 24

		return __vsrw(tmp, rShift); // shift right
	}

	static PXD_FORCE_INLINE Matrix34 load(const PxcMatrix34 &a)
	{
		Matrix34 rv;

		rv.base0 = load(a.base0);
		rv.base1 = load(a.base1);
		rv.base2 = load(a.base2);
		rv.base3 = load(a.base3);

		return rv;
	}


	/* stores */
	static PXD_FORCE_INLINE void store(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3(&pxcVec, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxcQuaternion &pxcQuat, const Vector4 &simdQuat)
	{
		XMStoreVector4(&pxcQuat, simdQuat);
	}

	static PXD_FORCE_INLINE void storeA(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3A(&pxcVec, simdVec);
	}

	//WARNING: Use with care as this writes to the dword after pxcVec.
	static PXD_FORCE_INLINE void store4A(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector4A(&pxcVec, simdVec);
	}

	static PXD_FORCE_INLINE void storeA(Vector4 &vec, const Vector4 &inVec)
	{
		XMStoreVector4A(&vec, inVec);
	}

	//Store the word in inVec corresponding to a, ie if a is 16byte aligned the x element of inVec is stored.
	// if a is set to aligned_address+4 then y is stored etc.
	static PXD_FORCE_INLINE void storeElementA(float &a, const Vector4 &inVec)
	{
		__stvewx(inVec, &a, 0);
	}

	static PXD_FORCE_INLINE void store(PxFloat &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxU32 &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		PxU32 a32;
		XMStoreScalar(&a32, simdVec);
		a = (PxU8)a32; //LHS
	}

	/* basic math */
	static PXD_FORCE_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAdd(a, b);
	}

	static PXD_FORCE_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorSubtract(a, b);
	}

	static PXD_FORCE_INLINE Vector4 negate(const Vector4 &a)
	{
		return XMVectorNegate(a);
	}

	static PXD_FORCE_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMultiply(a, b);
	}

	static PXD_FORCE_INLINE Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorMultiplyAdd(a, b, c);
	}

	static PXD_FORCE_INLINE Vector4 negMultiplySubtract(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorNegativeMultiplySubtract(a, b, c);
	}

	static PXD_FORCE_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return XMVectorReciprocal(a);
	}

	static PXD_FORCE_INLINE Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return XMVectorReciprocalSqrt(a);
	}

	// doesnt handle infinities, zero etc.
	static PXD_FORCE_INLINE void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		rsq = XMVectorReciprocalSqrt(a);
		sq = XMVectorMultiply(a, rsq);
	}

	static PXD_FORCE_INLINE Vector4 lengthSq(const Vector4 &a)
	{
		return XMVector3LengthSq(a);
	}

	static PXD_FORCE_INLINE Vector4 length(const Vector4 &a)
	{
		return XMVector3Length(a);
	}

	// 3 component dot product
	static PXD_FORCE_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Dot(a, b);
	}

	static PXD_FORCE_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Dot(a, b);
	}

	static PXD_FORCE_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Cross(a, b);
	}

	static PXD_FORCE_INLINE void sqrtAndRcpSqrtEst(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		rsq = XMVectorReciprocalSqrtEst(a);
		sq = XMVectorMultiply(a, rsq);
	}

	static PXD_FORCE_INLINE Vector4 normalizeSafe(const Vector4 &n, Vector4 &nLength)
	{
		Vector4 mask, length, lengthRcp,d;

		d = dot(n, n);
		sqrtAndRcpSqrtEst(d, length, lengthRcp);

		mask = notEqual(d, zero());
        nLength = and4(mask, length);
		return and4(mask, multiply(n, lengthRcp));
	}

	static PXD_FORCE_INLINE Vector4 abs(const Vector4 &a)
	{
		return XMVectorAbs(a);
	}

	static PXD_FORCE_INLINE Vector4 zero()
	{
		return XMVectorZero();
	}

	static PXD_FORCE_INLINE Vector4 one()
	{
		return XMVectorSplatOne();
	}

	static PXD_FORCE_INLINE Vector4 minusOne()
	{
		static const XMVECTOR a = {-1.0f, -1.0f, -1.0f, -1.0f};
		return XMLoadVector4A(&a);
	}

	static PXD_FORCE_INLINE Vector4 two()
	{
		static const XMVECTOR a = {2.0f, 2.0f, 2.0f, 2.0f};
		return XMLoadVector4A(&a);
	}

	static PXD_FORCE_INLINE Vector4 half()
	{
		static const XMVECTOR a = {0.5f, 0.5f, 0.5f, 0.5f};
		return XMLoadVector4A(&a);
	}

	static PXD_FORCE_INLINE Vector4 eps()
	{
		static const XMVECTOR e = {PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON};
		return XMLoadVector4A(&e);
	}
	static PXD_FORCE_INLINE Vector4 eps6()
	{
		static const XMVECTOR e = {1e-6f, 1e-6f, 1e-6f, 1e-6f};
		return XMLoadVector4A(&e);
	}

	static PXD_FORCE_INLINE Vector4 xMask()
	{
		static const XMVECTORI x = {0xffFFffFF, 0, 0, 0};
		return XMLoadVector4A(&x);
	}

	static PXD_FORCE_INLINE Vector4 yMask()
	{
		static const XMVECTORI y = {0, 0xffFFffFF, 0, 0};
		return XMLoadVector4A(&y);
	}

	static PXD_FORCE_INLINE Vector4 zMask()
	{
		static const XMVECTORI z = {0, 0, 0xffFFffFF, 0};
		return XMLoadVector4A(&z);
	}

	static PXD_FORCE_INLINE Vector4 wMask()
	{
		static const XMVECTORI w = {0, 0, 0, 0xffFFffFF};
		return XMLoadVector4A(&w);
	}

	static PXD_FORCE_INLINE Vector4 xyzMask()
	{
		static const XMVECTORI xyz = {0xffFFffFF, 0xffFFffFF, 0xffFFffFF, 0};
		return XMLoadVector4A(&xyz);
	}

	static PXD_FORCE_INLINE Vector4 allSet()
	{
		return __vspltisw(-1); // splat 0xffFFffFF
	}

	static PXD_FORCE_INLINE Vector4 floatMin()
	{
		static const XMVECTOR a = {-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX};
		return XMLoadVector4A(&a);
	}

	static PXD_FORCE_INLINE Vector4 floatMax()
	{
		static const XMVECTOR a = {PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX};
		return XMLoadVector4A(&a);
	}

	static PXD_FORCE_INLINE Vector4 signMask()
	{
		XMVECTOR signMask = __vspltisw(-1);
		signMask = __vslw(signMask, signMask);

		return signMask;
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

	static PXD_FORCE_INLINE Vector4 transform(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &basis3, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiplyAdd(basis0, X, basis3);
		result = multiplyAdd(basis1, Y, result);
		result = multiplyAdd(basis2, Z, result);
		
		return result;
	}

	static PXD_FORCE_INLINE Vector4 transform(const Matrix34 &mat, const Vector4 &other)
	{
		return transform(mat.base0, mat.base1, mat.base2, mat.base3, other);
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

	static PXD_FORCE_INLINE void transpose3x3(const Vector4 &inA, const Vector4 &inB, const Vector4 &inC, 
		Vector4 &outA, Vector4 &outB, Vector4 &outC)
	{
		Vector4 tmp0, tmp1, tmp2, tmp3;

		//modified 4x4 transpose, can probably work out faster version for 3x3??
		tmp0 = __vmrghw(inA, inC);
		tmp1 = XMVectorSwizzle(inB, 0, 0, 1, 1);
		tmp2 = __vmrglw(inA, inC);
		tmp3 = XMVectorSwizzle(inB, 2, 2, 3, 3);

		outA = __vmrghw(tmp0, tmp1);
		outB = __vmrglw(tmp0, tmp1);
		outC = __vmrghw(tmp2, tmp3);
	}


	static PXD_FORCE_INLINE Matrix34 multiply(const Matrix34 &a, const Matrix34 &b)
	{
		Matrix34 rv;

		rv.base0 = rotate(a.base0, a.base1, a.base2, b.base0);
		rv.base1 = rotate(a.base0, a.base1, a.base2, b.base1);
		rv.base2 = rotate(a.base0, a.base1, a.base2, b.base2);
		rv.base3 = transform(a.base0, a.base1, a.base2, a.base3, b.base3);

		return rv;
	}

	static PXD_FORCE_INLINE Matrix34 inverseRT(const Matrix34 &a)
	{
		Matrix34 rv;

		transpose3x3(a.base0, a.base1, a.base2,
			rv.base0, rv.base1, rv.base2);

		rv.base3 = rotateInv(a.base0, a.base1, a.base2, negate(a.base3));

		return rv;
	}

	/* Min/max , per component */
	static PXD_FORCE_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMax(a, b);
	}

	static PXD_FORCE_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMin(a, b);
	}


	static PXD_FORCE_INLINE Vector4 clamp(const Vector4 &a,const Vector4 &high, const Vector4 &low)
	{
		return min(high, max(low, a));
	}

	static PXD_FORCE_INLINE Vector4 insertW(const Vector4 &a, const Vector4 &insMe)
	{
		return XMVectorInsert(a, insMe, 0, 0, 0, 0, 1);
	}

	static PXD_FORCE_INLINE void unpack3to4(const Vector4 &inA, const Vector4 &inB, const Vector4 &inC,
		Vector4 &outA, Vector4 &outB, Vector4 &outC, Vector4 &outD)
	{
		//we have 4, 3 component vectors packed into 4 vector4s.

		outA = inA;
		outB = XMVectorShiftLeft(inA, inB, 3);
		outC = XMVectorShiftLeft(inB, inC, 2);
		outD = XMVectorShiftLeft(inC, inC, 1);
	}


	/* splatting */
	static PXD_FORCE_INLINE Vector4 splatX(const Vector4 &a)
	{
		return XMVectorSplatX(a);
	}

	static PXD_FORCE_INLINE Vector4 splatY(const Vector4 &a)
	{
		return XMVectorSplatY(a);
	}

	static PXD_FORCE_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return XMVectorSplatZ(a);
	}

	static PXD_FORCE_INLINE Vector4 splatW(const Vector4 &a)
	{
		return XMVectorSplatW(a);
	}

	/* permutations and selects */
	static PXD_FORCE_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 0, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 0, 0, 0, 1, 0);
		return tmp;
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static PXD_FORCE_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 3, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 2, 0, 0, 1, 0);
		return tmp;
	}

	static PXD_FORCE_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vY, vX, 1, 1, 0, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 3, 0, 0, 1, 0);
		return tmp;
	}

	static PXD_FORCE_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vZ, vY, 1, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vX, 2, 1, 0, 0, 0);
		return tmp;
	}


	static PXD_FORCE_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		return XMVectorPermute(a, b, XMVectorPermuteControl(e0, e1, e2, e3));
	}

	static PXD_FORCE_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return XMVectorSelect(a, b, control);
	}


		//fast immediate permutes with one vector. (allows vpermwi)
	/////
	static PXD_FORCE_INLINE Vector4 permuteXXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 0, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXXY(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 0, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 0, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteXYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 1, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXYY(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 1, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 1, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteXZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 2, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXZY(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 2, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteXZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 2, 2, 3); }



	static PXD_FORCE_INLINE Vector4 permuteYXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 0, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteYXY(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 0, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteyXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 0, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteYYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 1, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteYYY(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 1, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteYYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 1, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteYZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 2, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteYZY(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 2, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteYZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 2, 2, 3); }

	
	
	static PXD_FORCE_INLINE Vector4 permuteZXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 0, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZXY(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 0, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 0, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteZYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 1, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZYY(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 1, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 1, 2, 3); }

	static PXD_FORCE_INLINE Vector4 permuteZZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 2, 0, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZZY(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 2, 1, 3); }
	static PXD_FORCE_INLINE Vector4 permuteZZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 2, 2, 3); }

	static PXD_FORCE_INLINE void swap(Vector4 &a, Vector4 &b)
	{
		Vector4 tmp = a;
		a = b;
		b = tmp;
	}

	static PXD_FORCE_INLINE void swap(Vector4 &a, Vector4 &b, const Vector4 &mask)
	{
		//swap is mask is true
		Vector4 tmp = select(b , a, mask);
		a = select(a, b, mask);
		b = tmp;
	}

	/* float comparisons (bitmasks)*/
	static PXD_FORCE_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorEqual(a, b);
	}

	static PXD_FORCE_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqual(a, b);
	}

	static PXD_FORCE_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLess(a, b);
	}
	
	static PXD_FORCE_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreater(a, b);
	}

	static PXD_FORCE_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLessOrEqual(a, b);
	}

	static PXD_FORCE_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreaterOrEqual(a, b);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static PXD_FORCE_INLINE PxIBool equalBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Equal(a, b);
	}

	static PXD_FORCE_INLINE PxIBool notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqual(a, b);
	}

	static PXD_FORCE_INLINE PxIBool lessBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Less(a, b);
	}

	static PXD_FORCE_INLINE PxIBool greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Greater(a, b);
	}

	static PXD_FORCE_INLINE PxIBool lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3LessOrEqual(a, b);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3GreaterOrEqual(a, b);
	}

	static PXD_FORCE_INLINE PxIBool lessEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(a, b);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(a, b);
	}

	/* float comparisons bool, X component */

	static PXD_FORCE_INLINE PxIBool equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Equal(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static PXD_FORCE_INLINE PxIBool notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4NotEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static PXD_FORCE_INLINE PxIBool lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Less(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static PXD_FORCE_INLINE PxIBool greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Greater(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static PXD_FORCE_INLINE PxIBool lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	/* int comparisons (bitmasks) */
	static PXD_FORCE_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqualInt(a, b);
	}

	/* int comparisons bool (first 3 components)*/
	static PXD_FORCE_INLINE PxIBool intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqualInt(a, b);
	}

	/* bitwise logical operations */
	static PXD_FORCE_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndInt(a, b);
	}

	static PXD_FORCE_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorOrInt(a, b);
	}

	static PXD_FORCE_INLINE Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorXorInt(a, b);
	}

	static PXD_FORCE_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndCInt(a, b);
	}

private:
	PxcSimd(){}
};


#endif
