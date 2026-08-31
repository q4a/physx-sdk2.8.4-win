#ifndef NX_SIMD_XBOX_H
#define NX_SIMD_XBOX_H

#include "Nx.h"
#include "NxVec3.h"
#include "NxPlane.h"
#include "NxMath.h"
#include <float.h>

/*!
Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. NxVec3s will be read/written as 4 component vectors!

NOTE: NxSimd mirrors PxcSimd. In future we should probably use the same source somehow.
*/

class NxSimd
{
public:
	typedef XMVECTOR Vector4;

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
		return XMLoadVector3(&pxcVec);
	}

	static NX_INLINE Vector4 loadW1(const NxVec3 &pxcVec)
	{
		static const XMVECTOR w1 = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const XMVECTORI control  = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};

		return XMVectorSelect(XMLoadVector3(&pxcVec), XMLoadVector4A(&w1), XMLoadVector4A(&control));
	}

	static NX_INLINE Vector4 loadA(const NxVec3 &pxcVec)
	{
		return XMLoadVector3A(&pxcVec);
	}

	static NX_INLINE Vector4 load(const NxPlane &pxcPlane)
	{
		return XMLoadVector4(&pxcPlane);
	}

	static NX_INLINE Vector4 load(const NxReal &a)
	{
		return XMLoadScalar(&a);
	}

	static NX_INLINE Vector4 load(const NxU32 &a)
	{
		return XMLoadScalar(&a);
	}

	static NX_INLINE Vector4 load(const PxU8 &a)
	{
		__vector4 tmp = __lvlx(&a, 0); // load to left of register
		__vector4 rShift = __vspltisw(-8); //shift by 24

		return __vsrw(tmp, rShift); // shift right
	}

	static NX_INLINE Vector4 load3(const NxReal *a)
	{
		return XMLoadVector3(a);
	}

	static NX_INLINE Vector4 load4(const NxReal *a)
	{
		return XMLoadVector4(a);
	}

	static NX_INLINE Vector4 load4A(const NxReal &a)
	{
		return XMLoadVector4A(&a);
	}

	static NX_INLINE Vector4 load4A(const NxU32 &a)
	{
		return XMLoadVector4A(&a);
	}

	/* stores */
	static NX_INLINE void store(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3(&pxcVec, simdVec);
	}

	static NX_INLINE void storeA(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3A(&pxcVec, simdVec);
	}
	static NX_INLINE void storeA4(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector4A(&pxcVec, simdVec);
	}

	static NX_INLINE void store(NxReal &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

	static NX_INLINE void store(NxU32 &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

	static NX_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		NxU32 a32;
		XMStoreScalar(&a32, simdVec);
		a = (PxU8)a32; //LHS
	}

	/*
	Conversion operators
	*/

	static NX_INLINE Vector4 uintToFloat(const Vector4 &a)
	{
		return XMConvertVectorUIntToFloat(a, 0);
	}

	/* basic math */
	static NX_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAdd(a, b);
	}

	static NX_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorSubtract(a, b);
	}

	static PXD_FORCE_INLINE Vector4 negate(const Vector4 &a)
	{
		return XMVectorNegate(a);
	}

	static NX_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMultiply(a, b);
	}

	static NX_INLINE Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorMultiplyAdd(a, b, c);
	}

	static NX_INLINE Vector4 negMultiplySubtract(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorNegativeMultiplySubtract(a, b, c);
	}

	static NX_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return XMVectorReciprocal(a);
	}
	
	static NX_INLINE Vector4 reciprocalEst(const Vector4 &a)
	{
		return XMVectorReciprocalEst(a);
	}

	static NX_INLINE Vector4 reciprocalSafe(const Vector4 &a)
	{
		Vector4 mask = notEqual(a, zero());
		Vector4 result = reciprocal(a);
		return and4(result, mask);
	}


	static NX_INLINE Vector4 pow(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorPow(a, b);
	}

	// 3 component dot product
	static NX_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Dot(a, b);
	}

	static NX_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Dot(a, b);
	}

	static NX_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Cross(a, b);
	}

	static NX_INLINE Vector4 abs(const Vector4 &a)
	{
		return XMVectorAbs(a);
	}

	// should handle a==0 correctly.
	static NX_INLINE Vector4 sqrt(const Vector4 &a)
	{
		return XMVectorSqrt(a);
	}

	static NX_INLINE Vector4 magnitudeSafe(const Vector4 &n, Vector4 &oneH)
	{
		Vector4 d, mask, sq, rsq;

		d = dot(n, n);
		mask = notEqual(d, zero());
		sqrtAndRcpSqrt(d, sq, rsq);

		oneH = and4(mask, rsq);
		return and4(mask, sq);
	}

	// doesnt handle infinities, zero etc.
	static NX_INLINE void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		rsq = XMVectorReciprocalSqrt(a);
		sq = XMVectorMultiply(a, rsq);
	}
	
	// doesnt handle infinities, zero etc.
	static NX_INLINE void sqrtAndRcpSqrtEst(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		rsq = XMVectorReciprocalSqrtEst(a);
		sq = XMVectorMultiply(a, rsq);
	}

	static NX_INLINE Vector4 normalizeSafe(const Vector4 &n, Vector4 &nLength)
	{
		Vector4 mask, length, lengthRcp,d;

		d = dot(n, n);
		sqrtAndRcpSqrtEst(d, length, lengthRcp);

		mask = notEqual(d, zero());
        nLength = and4(mask, length);
		return and4(mask, multiply(n, lengthRcp));
	}

	static NX_INLINE Vector4 normalizeRecip(const Vector4 &n, Vector4 &rcpLength)
	{
		Vector4 mask,d;
		
		d = dot(n, n);
		rcpLength = XMVectorReciprocalSqrt(d);
		mask = notEqual(d, zero());
		rcpLength = and4(mask, rcpLength);
		
		return multiply(n, rcpLength);
	}

	static NX_INLINE Vector4 clamp(const Vector4 &a, const Vector4 &hi, const Vector4 &lo)
	{
		return min(hi, max(lo, a));
	}

	static NX_INLINE Vector4 acos(const Vector4 &a)
	{
		return XMVectorACos(a);
	}

	static NX_INLINE Vector4 zero()
	{
		return XMVectorZero();
	}

	static NX_INLINE Vector4 one()
	{
		return XMVectorSplatOne();
	}

	static NX_INLINE Vector4 minusOne()
	{
		static const XMVECTOR o = {-1.0f, -1.0f, -1.0f, -1.0f};
		return XMLoadVector4A(&o);
	}

	static NX_INLINE Vector4 oneX()
	{
		static const XMVECTOR o = {1.0f, 0.0f, 0.0f, 0.0f};
		return XMLoadVector4A(&o);
	}

	static NX_INLINE Vector4 oneY()
	{
		static const XMVECTOR o = {0.0f, 1.0f, 0.0f, 0.0f};
		return XMLoadVector4A(&o);
	}

	static NX_INLINE Vector4 oneZ()
	{
		static const XMVECTOR o = {0.0f, 0.0f, 1.0f, 0.0f};
		return XMLoadVector4A(&o);
	}

	static NX_INLINE Vector4 half()
	{
		//__vspltisw(1)
		//__vcfsx(H, 1);

		static const XMVECTOR h = {0.5f, 0.5f, 0.5f, 0.5f};
		return XMLoadVector4A(&h);
	}

	static NX_INLINE Vector4 two()
	{
		static const XMVECTOR a = {2.0f, 2.0f, 2.0f, 2.0f};
		return XMLoadVector4A(&a);
	}

	static NX_INLINE Vector4 eps()
	{
		static const XMVECTOR e = {PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON, PXD_FLOAT_EPSILON};
		return XMLoadVector4A(&e);
	}
	static NX_INLINE Vector4 eps6()
	{
		static const XMVECTOR e = {1e-6f, 1e-6f, 1e-6f, 1e-6f};
		return XMLoadVector4A(&e);
	}

	static NX_INLINE Vector4 eps4()
	{
		static const XMVECTOR e = {1e-4f, 1e-4f, 1e-4f, 1e-4f};
		return XMLoadVector4A(&e);
	}

	static NX_INLINE Vector4 allSet()
	{
		return __vspltisw(-1); // splat 0xffFFffFF
	}

	static NX_INLINE Vector4 floatMin()
	{
		static const XMVECTOR a = {-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX};
		return XMLoadVector4A(&a);
	}
    
	static NX_INLINE Vector4 floatMax()
	{
		static const XMVECTOR a = {PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX};
		return XMLoadVector4A(&a);
	}

	static NX_INLINE Vector4 signMask()
	{
		XMVECTOR signMask = __vspltisw(-1);
		signMask = __vslw(signMask, signMask);

		return signMask;
	}

	static NX_INLINE Vector4 rotateInv(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, const Vector4 &other)
	{
		return mergeXYZ(dot(basis0, other),
						dot(basis1, other),
						dot(basis2, other));
	}

	static NX_INLINE Vector4 lerpV(const Vector4 &v0,const Vector4 &v1,const Vector4 &t)
	{
		return XMVectorLerpV(v0,v1,t);	
	}

	static NX_INLINE Vector4 lerp(const Vector4 &v0,const Vector4 &v1,const NxReal &t)
	{
		return XMVectorLerp(v0,v1,t);	
	}

	/* Min/max , per component */
	static NX_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMax(a, b);
	}

	static NX_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMin(a, b);
	}


	/* Min/max , which component */
	static NX_INLINE NxReal maxComp(const Vector4& v, Vector4& which) 
	{
		NxReal R = v.x;
		which = oneX();
		if(R < v.y)
		{
			R = v.y;
			which = oneY();
		}
		if(R < v.z)
		{
			R =  v.z;
			which = oneZ();
		}
		return R;
	}

	static NX_INLINE NxReal minComp(const Vector4& v, Vector4& which) 
	{

		NxReal R = v.x;
		which = oneX();
		if(R > v.y)
		{
			R = v.y;
			which = oneY();
		}
		if(R > v.z)
		{
			R =  v.z;
			which = oneZ();
		}
		return R;
	}


	/* splatting */
	static NX_INLINE Vector4 splatX(const Vector4 &a)
	{
		return XMVectorSplatX(a);
	}

	static NX_INLINE Vector4 splatY(const Vector4 &a)
	{
		return XMVectorSplatY(a);
	}

	static NX_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return XMVectorSplatZ(a);
	}

	static NX_INLINE Vector4 splatW(const Vector4 &a)
	{
		return XMVectorSplatW(a);
	}

	/* permutations and selects */
	static NX_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 0, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 0, 0, 0, 1, 0);
		return tmp;
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static NX_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 3, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 2, 0, 0, 1, 0);
		return tmp;
	}

	static NX_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vY, vX, 1, 1, 0, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 3, 0, 0, 1, 0);
		return tmp;
	}

	static NX_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vZ, vY, 1, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vX, 2, 1, 0, 0, 0);
		return tmp;
	}


	static NX_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		return XMVectorPermute(a, b, XMVectorPermuteControl(e0, e1, e2, e3));
	}

	static NX_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return XMVectorSelect(a, b, control);
	}

	template<int x, int y, int z, int w>
    static Vector4 VectorSwizzle(const Vector4& v)
    {
         return XMVectorSwizzle(v, x, y, z, w);
    }

	static NX_INLINE NX_BOOL InBoundsBool(const Vector4 &a, const Vector4 &bounds)
	{
		return XMVector3InBounds(a,bounds);
	}

	/* float comparisons (bitmasks)*/
	static NX_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorEqual(a, b);
	}

	static NX_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqual(a, b);
	}

	static NX_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLess(a, b);
	}
	
	static NX_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreater(a, b);
		
	}
    
	static NX_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLessOrEqual(a, b);
	}

	static NX_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreaterOrEqual(a, b);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static NX_INLINE NX_BOOL equalBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Equal(a, b);
	}

	static NX_INLINE NX_BOOL notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqual(a, b);
	}

	static NX_INLINE NX_BOOL lessBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Less(a, b);
	}

	static NX_INLINE NX_BOOL greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Greater(a, b);
	}

	static NX_INLINE NX_BOOL lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3LessOrEqual(a, b);
	}

	static NX_INLINE NX_BOOL greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3GreaterOrEqual(a, b);
	}

	/* float comparisons (bool, for jumps)- all 4 components */	
	static NX_INLINE NX_BOOL equal4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Equal(a, b);
	}

	static NX_INLINE NX_BOOL notEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4NotEqual(a, b);
	}

	static NX_INLINE NX_BOOL less4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Less(a, b);
	}

	static NX_INLINE NX_BOOL greater4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Greater(a, b);
	}

	static NX_INLINE NX_BOOL lessEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(a, b);
	}

	static NX_INLINE NX_BOOL greaterEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(a, b);
	}

	/* float comparisons bool, X component */

	static NX_INLINE NX_BOOL equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Equal(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static NX_INLINE NX_BOOL notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4NotEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static NX_INLINE NX_BOOL lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Less(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static NX_INLINE NX_BOOL greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Greater(XMVectorSplatX(a), XMVectorSplatX(b));
	}
	
	static NX_INLINE NX_BOOL lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static NX_INLINE NX_BOOL greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	/* int comparisons (bitmasks) */
	static NX_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqualInt(a, b);
	}

	/* int comparisons bool (first 3 components)*/
	static NX_INLINE NX_BOOL intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqualInt(a, b);
	}

	/* bitwise logical operations */
	static NX_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndInt(a, b);
	}

	static NX_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorOrInt(a, b);
	}

	static NX_INLINE Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorXorInt(a, b);
	}

	static NX_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndCInt(a, b);		
	}

	static NX_INLINE NX_BOOL anyTrue(const Vector4 &a)
	{
		XMVECTOR C;
    
		 // Duplicate the fourth element from the first element.
		 C = XMVectorSwizzle( a, 0, 1, 2, 0 );
    
		 return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );		
	}
	
	struct Matrix34
	{
	Matrix34(){}
	Matrix34(const XMMATRIX& m) : M(m){}
	Matrix34(const NxMat34& m)	{load(m);}
	Matrix34(Vector4 &a, Vector4 &b, Vector4 &c, Vector4 &d)
	{
		M.r[0] = a;
		M.r[1] = b;
		M.r[2] = c;
		M.r[3] = d;
	}

	Matrix34& operator = (const NxMat34& m) {load(m); return *this;}

	NX_INLINE void load(const NxMat34 &a)
	{
		M.r[0] = NxSimd::load(a.M.getRow(0));
		M.r[1] = NxSimd::load(a.M.getRow(1));
		M.r[2] = NxSimd::load(a.M.getRow(2));
		M.r[3] = NxSimd::load(a.t);
	}

	NX_INLINE Vector4 rotate(const Vector4 &other) const
	{

		Vector4 result;
#if 0
		Vector4 X = NxSimd::splatX(other);
		Vector4 Y = NxSimd::splatY(other);
		Vector4 Z = NxSimd::splatZ(other);

		result = NxSimd::multiply   (M.r[0], X);
		result = NxSimd::multiplyAdd(M.r[1], Y, result);
		result = NxSimd::multiplyAdd(M.r[2], Z, result);
#else
		Vector4 X = __vspltw(other, 0);
		Vector4 Y = __vspltw(other, 1);
		Vector4 Z = __vspltw(other, 2);

		result = __vmulfp (M.r[0], X);
		result = __vmaddfp(M.r[1], Y, result);
		result = __vmaddfp(M.r[2], Z, result);
#endif

		return result;
	}


	NX_INLINE Vector4 rotateInv(const Vector4 &other) const
	{
		return NxSimd::mergeXYZ(
			NxSimd::dot(M.r[0], other),
			NxSimd::dot(M.r[1], other),
			NxSimd::dot(M.r[2], other));
	}

	NX_INLINE Vector4 transform(const Vector4 &other) const
	{
		Vector4 result;
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);
		
		result = multiplyAdd(M.r[0], X, M.r[3]);
		result = multiplyAdd(M.r[1], Y, result);
		result = multiplyAdd(M.r[2], Z, result);

		return result;
	}

	NX_INLINE void transpose3x3(XMVECTOR &outA, XMVECTOR &outB, XMVECTOR &outC)
	{
		XMVECTOR tmp0, tmp1, tmp2, tmp3;

		//modified 4x4 transpose, can probably work out faster version for 3x3??
		tmp0 = __vmrghw(M.r[0], M.r[2]);
		tmp1 = XMVectorSwizzle(M.r[1], 0, 0, 1, 1);
		tmp2 = __vmrglw(M.r[0], M.r[2]);
		tmp3 = XMVectorSwizzle(M.r[1], 2, 2, 3, 3);

		outA = __vmrghw(tmp0, tmp1);
		outB = __vmrglw(tmp0, tmp1);
		outC = __vmrghw(tmp2, tmp3);
	}


	NX_INLINE Vector4 multiply(const Vector4 &other) const
	{
		Vector4 tmp0 = rotateInv(other);
		return  NxSimd::add(tmp0, M.r[3]);
	}

	NX_INLINE Matrix34 multiply(const Matrix34 &b) const
	{
		Matrix34 rv;

		rv.M.r[0] = rotate(b.M.r[0]);
		rv.M.r[1] = rotate(b.M.r[1]);
		rv.M.r[2] = rotate(b.M.r[2]);
		rv.M.r[3] = transform(b.M.r[3]);

		return rv;
	}

	NX_INLINE Matrix34 inverseRT()
	{
		Matrix34 rv;

		transpose3x3(rv.M.r[0], rv.M.r[1], rv.M.r[2]);

		rv.M.r[3] = rotateInv(-M.r[3]);

		return rv;
	}

	NX_INLINE Vector4 multiplyByInverseRT(const Vector4 &v)  const
	{
		//dst = M' * v - M' * t = M' * (v - t)
		return rotate(v - M.r[3]);
	}

	XMMATRIX M;
	};

	static NX_INLINE Matrix34 MatrixIdentity()
	{
		return XMMatrixIdentity();
	}	

private:
	NxSimd(){}
};


#endif

