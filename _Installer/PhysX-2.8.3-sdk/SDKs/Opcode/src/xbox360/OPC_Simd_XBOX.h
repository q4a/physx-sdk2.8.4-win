#ifndef OPC_SIMD_XBOX_H
#define OPC_SIMD_XBOX_H

/*
This is the opcode version of PxcSimd, it is painful having PxcSimd, NxSimd and OpcSimd but it is duplicated so
we dont introduce dependancies. PxcSimd is considered the master file and OpcSimd/NxSimd are just versions altered to
conform to the respective library.
*/

/*!
Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. Points will be read/written as 4 component vectors!
*/


#define OPC_SIMD_0X XM_PERMUTE_0X
#define OPC_SIMD_0Y XM_PERMUTE_0Y
#define OPC_SIMD_0Z XM_PERMUTE_0Z
#define OPC_SIMD_0W XM_PERMUTE_0W
#define OPC_SIMD_1X XM_PERMUTE_1X
#define OPC_SIMD_1Y XM_PERMUTE_1Y
#define OPC_SIMD_1Z XM_PERMUTE_1Z
#define OPC_SIMD_1W XM_PERMUTE_1W


#define OPC_SIMD_PERMUTE(name, a, b, c, d) static const XMVECTORI name##_const = {a, b, c, d}; \
	OpcSimd::Vector4 name = XMLoadVector4A(&##name##_const);

class OpcSimd
{
public:
	typedef XMVECTOR Vector4;

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
		// try this unsafe version?
		return XMLoadVector4(&pxcVec);
		//return XMLoadVector3(&pxcVec);
	}

	static inline_ Vector4 loadW1(const Point &pxcVec)
	{
		static const XMVECTOR w1 = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const XMVECTORI control  = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};

		return XMVectorSelect(XMLoadVector3(&pxcVec), XMLoadVector4A(&w1), XMLoadVector4A(&control));
	}

	static inline_ Vector4 loadA(const Point &pxcVec)
	{
		return XMLoadVector3A(&pxcVec);
	}

	static inline_ Vector4 load(const Plane &Plane)
	{
		return XMLoadVector4(&Plane);
	}

	static inline_ Vector4 load(const Vector4 &vec4)
	{
		return XMLoadVector4(&vec4);
	}

	/*static inline_ Vector4 load(const PxcQuaternion &pxcQuat)
	{
		return XMLoadVector4(&pxcQuat);
	}*/

	static inline_ Vector4 load(const float &a)
	{
		return XMLoadScalar(&a);
	}

	static inline_ Vector4 load(const udword &a)
	{
		return XMLoadScalar(&a);
	}

	static inline_ Vector4 loadToFloat3(const sword *a)
	{
		// Use compiler intrinsics since XM assert on -32k, but we may not care about w.
		__vector4 right = __lvrx(a, 8);
		__vector4 left = __lvlx(a, 0);
		right = __vsldoi(right, right, 2 << 2);
		__vector4 result = __vor(left, right);
		result = __vupkhsh(result);
		result = __vcfsx(result, 0);

		return result;
	}

	static inline_ Vector4 loadToFloat3(const uword *a)
	{
		__vector4 right = __lvrx(a, 8);
		__vector4 left = __lvlx(a, 0);
		right = __vsldoi(right, right, 2 << 2);
		__vector4 zer = __vspltisw(0);
		__vector4 result = __vor(left, right);
		result = __vmrghh(zer, result);
		result = __vcfux(result, 0);

		return result;
	}

	/*static inline_ Vector4 load(const PxU8 &a)
	{
		__vector4 tmp = __lvlx(&a, 0); // load to left of register
		__vector4 rShift = __vspltisw(-8); //shift by 24

		return __vsrw(tmp, rShift); // shift right
	}*/

	/* stores */
	static inline_ void store(Point &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3(&pxcVec, simdVec);
	}

/*	static inline_ void store(PxcQuaternion &pxcQuat, const Vector4 &simdQuat)
	{
		XMStoreVector4(&pxcQuat, simdQuat);
	}*/

	static inline_ void storeA(Point &pxcVec, const Vector4 &simdVec)
	{
		XMStoreVector3A(&pxcVec, simdVec);
	}

	static inline_ void store(float &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

	static inline_ void store(udword &a, const Vector4 &simdVec)
	{
		XMStoreScalar(&a, simdVec);
	}

/*	static inline_ void store(PxU8 &a, const Vector4 &simdVec)
	{
		udword a32;
		XMStoreScalar(&a32, simdVec);
		a = (PxU8)a32; //LHS
	}*/

	/* basic math */
	static inline_ Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAdd(a, b);
	}

	static inline_ Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorSubtract(a, b);
	}

	static inline_ Vector4 negate(const Vector4 &a)
	{
		return XMVectorNegate(a);
	}

	static inline_ Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMultiply(a, b);
	}

	static inline_ Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorMultiplyAdd(a, b, c);
	}

	static inline_ Vector4 negMultiplySubtract(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return XMVectorNegativeMultiplySubtract(a, b, c);
	}

	static inline_ Vector4 reciprocal(const Vector4 &a)
	{
		return XMVectorReciprocal(a);
	}

	static inline_ Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return XMVectorReciprocalSqrt(a);
	}

	// doesnt handle infinities, zero etc.
	static inline_ void sqrtAndRcpSqrt(const Vector4 &a, Vector4 &sq, Vector4 &rsq)
	{
		rsq = XMVectorReciprocalSqrt(a);
		sq = XMVectorMultiply(a, rsq);
	}

	static inline_ Vector4 sqrt(const Vector4 &a)
	{
		return XMVectorSqrt(a);
	}

	// 3 component dot product
	static inline_ Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Dot(a, b);
	}

	static inline_ Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Dot(a, b);
	}

	static inline_ Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Cross(a, b);
	}

	static inline_ Vector4 abs(const Vector4 &a)
	{
		return XMVectorAbs(a);
	}

	static inline_ Vector4 zero()
	{
		return XMVectorZero();
	}

	static inline_ Vector4 one()
	{
		return XMVectorSplatOne();
	}

	static inline_ Vector4 minusOne()
	{
		static const XMVECTOR a = {-1.0f, -1.0f, -1.0f, -1.0f};
		return XMLoadVector4A(&a);
	}

	static inline_ Vector4 two()
	{
		static const XMVECTOR a = {2.0f, 2.0f, 2.0f, 2.0f};
		return XMLoadVector4A(&a);
	}

	static inline_ Vector4 half()
	{
		static const XMVECTOR a = {0.5f, 0.5f, 0.5f, 0.5f};
		return XMLoadVector4A(&a);
	}
/*	static inline_ Vector4 eps()
	{
		static const XMVECTOR e = {OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON, OPC_FLOAT_EPSILON};
		return XMLoadVector4A(&e);
	}*/
	static inline_ Vector4 eps6()
	{
		static const XMVECTOR e = {1e-6f, 1e-6f, 1e-6f, 1e-6f};
		return XMLoadVector4A(&e);
	}

	static inline_ Vector4 xMask()
	{
		static const XMVECTORI x = {0xffFFffFF, 0, 0, 0};
		return XMLoadVector4A(&x);
	}

	static inline_ Vector4 yMask()
	{
		static const XMVECTORI y = {0, 0xffFFffFF, 0, 0};
		return XMLoadVector4A(&y);
	}

	static inline_ Vector4 zMask()
	{
		static const XMVECTORI z = {0, 0, 0xffFFffFF, 0};
		return XMLoadVector4A(&z);
	}

	static inline_ Vector4 wMask()
	{
		static const XMVECTORI w = {0, 0, 0, 0xffFFffFF};
		return XMLoadVector4A(&w);
	}

	static inline_ Vector4 xyzMask()
	{
		static const XMVECTORI xyz = {0xffFFffFF, 0xffFFffFF, 0xffFFffFF, 0};
		return XMLoadVector4A(&xyz);
	}

	static inline_ Vector4 signMask()
	{
		XMVECTOR signMask = __vspltisw(-1);
		signMask = __vslw(signMask, signMask);

		return signMask;
	}

	static inline_ Vector4 allSet()
	{
		return __vspltisw(-1); // splat 0xffFFffFF
	}

/*	static inline_ Vector4 floatMin()
	{
		static const XMVECTOR a = {-OPC_FLOAT_MAX, -OPC_FLOAT_MAX, -OPC_FLOAT_MAX, -OPC_FLOAT_MAX};
		return XMLoadVector4A(&a);
	}

	static inline_ Vector4 floatMax()
	{
		static const XMVECTOR a = {OPC_FLOAT_MAX, OPC_FLOAT_MAX, OPC_FLOAT_MAX, OPC_FLOAT_MAX};
		return XMLoadVector4A(&a);
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

	static inline_ Vector4 transform(const Vector4 &basis0, const Vector4 &basis1, const Vector4 &basis2, 
		const Vector4 &tran, const Vector4 &other)
	{
		Vector4 X = splatX(other);
		Vector4 Y = splatY(other);
		Vector4 Z = splatZ(other);

		Vector4 result = multiplyAdd(basis0, X, tran);
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

	static inline_ Vector4 pointOnLineSegNearPoint(const Vector4 &v0, const Vector4 &v1, const Vector4 &pt)
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

	/* Min/max , per component */
	static inline_ Vector4 max4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMax(a, b);
	}

	static inline_ Vector4 min4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorMin(a, b);
	}


	static inline_ Vector4 clamp(const Vector4 &a,const Vector4 &high, const Vector4 &low)
	{
		return min4(high, max4(low, a));
	}

	/* splatting */
	static inline_ Vector4 splatX(const Vector4 &a)
	{
		return XMVectorSplatX(a);
	}

	static inline_ Vector4 splatY(const Vector4 &a)
	{
		return XMVectorSplatY(a);
	}

	static inline_ Vector4 splatZ(const Vector4 &a)
	{
		return XMVectorSplatZ(a);
	}

	static inline_ Vector4 splatW(const Vector4 &a)
	{
		return XMVectorSplatW(a);
	}

	/* permutations and selects */
	static inline_ Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 0, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 0, 0, 0, 1, 0);
		return tmp;
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static inline_ Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vX, vY, 3, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 2, 0, 0, 1, 0);
		return tmp;
	}

	static inline_ Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vY, vX, 1, 1, 0, 0, 0);
		tmp = XMVectorInsert(tmp, vZ, 3, 0, 0, 1, 0);
		return tmp;
	}

	static inline_ Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		XMVECTOR tmp = XMVectorInsert(vZ, vY, 1, 0, 1, 0, 0);
		tmp = XMVectorInsert(tmp, vX, 2, 1, 0, 0, 0);
		return tmp;
	}

	static inline_ void transpose3x3(const Vector4 &inA, const Vector4 &inB, const Vector4 &inC, 
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

	static inline_ Vector4 insertW(const Vector4 &a, const Vector4 &insMe)
	{
		return XMVectorInsert(a, insMe, 0, 0, 0, 0, 1);
	}

	static inline_ Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const udword e0,const udword e1,const udword e2,const udword e3)
	{
		return XMVectorPermute(a, b, XMVectorPermuteControl(e0, e1, e2, e3));
	}

	static inline_ Vector4 permute(const Vector4 &a, const Vector4 &b, const Vector4 &ctrl)
	{
		return XMVectorPermute(a, b, ctrl);
	}

	//fast immediate permutes with one vector. (allows vpermwi)
	/////
	static inline_ Vector4 permuteXXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 0, 0, 3); }
	static inline_ Vector4 permuteXXY(const Vector4 &a) { return XMVectorSwizzle(a, 0, 0, 1, 3); }
	static inline_ Vector4 permuteXXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 0, 2, 3); }

	static inline_ Vector4 permuteXYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 1, 0, 3); }
	static inline_ Vector4 permuteXYY(const Vector4 &a) { return XMVectorSwizzle(a, 0, 1, 1, 3); }
	static inline_ Vector4 permuteXYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 1, 2, 3); }

	static inline_ Vector4 permuteXZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 2, 0, 3); }
	static inline_ Vector4 permuteXZY(const Vector4 &a) { return XMVectorSwizzle(a, 0, 2, 1, 3); }
	static inline_ Vector4 permuteXZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 0, 2, 2, 3); }



	static inline_ Vector4 permuteYXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 0, 0, 3); }
	static inline_ Vector4 permuteYXY(const Vector4 &a) { return XMVectorSwizzle(a, 1, 0, 1, 3); }
	static inline_ Vector4 permuteyXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 0, 2, 3); }

	static inline_ Vector4 permuteYYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 1, 0, 3); }
	static inline_ Vector4 permuteYYY(const Vector4 &a) { return XMVectorSwizzle(a, 1, 1, 1, 3); }
	static inline_ Vector4 permuteYYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 1, 2, 3); }

	static inline_ Vector4 permuteYZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 2, 0, 3); }
	static inline_ Vector4 permuteYZY(const Vector4 &a) { return XMVectorSwizzle(a, 1, 2, 1, 3); }
	static inline_ Vector4 permuteYZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 1, 2, 2, 3); }

	
	
	static inline_ Vector4 permuteZXX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 0, 0, 3); }
	static inline_ Vector4 permuteZXY(const Vector4 &a) { return XMVectorSwizzle(a, 2, 0, 1, 3); }
	static inline_ Vector4 permuteZXZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 0, 2, 3); }

	static inline_ Vector4 permuteZYX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 1, 0, 3); }
	static inline_ Vector4 permuteZYY(const Vector4 &a) { return XMVectorSwizzle(a, 2, 1, 1, 3); }
	static inline_ Vector4 permuteZYZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 1, 2, 3); }

	static inline_ Vector4 permuteZZX(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 2, 0, 3); }
	static inline_ Vector4 permuteZZY(const Vector4 &a) { return XMVectorSwizzle(a, 2, 2, 1, 3); }
	static inline_ Vector4 permuteZZZ(const Vector4 &a)	{ return XMVectorSwizzle(a, 2, 2, 2, 3); }

	static inline_ Vector4 permuteXYZX(const Vector4 &a) { return XMVectorSwizzle(a, 0, 1, 2, 0); }

	template<int x, int y, int z, int w>
	static inline_ Vector4 VectorSwizzle(const Vector4& v) { return XMVectorSwizzle(v, x, y, z, w); }

	/////
	static inline_ Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return XMVectorSelect(a, b, control);
	}


	/* float comparisons (bitmasks)*/
	static inline_ Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorEqual(a, b);
	}

	static inline_ Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqual(a, b);
	}

	static inline_ Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLess(a, b);
	}
	
	static inline_ Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreater(a, b);
	}

	static inline_ Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorLessOrEqual(a, b);
	}

	static inline_ Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorGreaterOrEqual(a, b);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static inline_ BOOL equalBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Equal(a, b);
	}

	static inline_ BOOL notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqual(a, b);
	}

	static inline_ BOOL lessBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Less(a, b);
	}

	static inline_ BOOL greaterBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3Greater(a, b);
	}

	static inline_ BOOL lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3LessOrEqual(a, b);
	}

	static inline_ BOOL greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3GreaterOrEqual(a, b);
	}

	static inline_ BOOL greaterEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(a, b);
	}

	static inline_ BOOL lessEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(a, b);

	}
	/* float comparisons bool, X component */

	static inline_ BOOL equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Equal(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4NotEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Less(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4Greater(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4LessOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4GreaterOrEqual(XMVectorSplatX(a), XMVectorSplatX(b));
	}

	static inline_ BOOL inBounds3Bool(const Vector4 &a, const Vector4 &bounds)
	{
		return XMVector3InBounds(a, bounds);
	}

	static inline_ BOOL inBounds4Bool(const Vector4 &a, const Vector4 &bounds)
	{
		return XMVector4InBounds(a, bounds);
	}

	static inline_ BOOL outBounds4Bool(const Vector4 &a, const Vector4 &bounds)
	{
		UINT CR;
		__vcmpbfpR(a, bounds, &CR);
		return XMComparisonAnyOutOfBounds(CR);
	}

	/* int comparisons (bitmasks) */
	static inline_ Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorNotEqualInt(a, b);
	}

	/* int comparisons bool (first 3 components)*/
	static inline_ BOOL intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector3NotEqualInt(a, b);
	}

	static inline_ BOOL intNotEqual4Bool(const Vector4 &a, const Vector4 &b)
	{
		return XMVector4NotEqualInt(a, b);
	}

	/* bitwise logical operations */
	static inline_ Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndInt(a, b);
	}

	static inline_ Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorOrInt(a, b);
	}

	static inline_ Vector4 xor4(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorXorInt(a, b);
	}

	static inline_ Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return XMVectorAndCInt(a, b);
	}

	static inline_ BOOL any3TrueBool(const Vector4 &a)
	{
		XMVECTOR C;
    
		 // Duplicate the fourth element from the first element.
		 C = XMVectorSwizzle( a, 0, 1, 2, 0 );
    
		 return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );		
	}

private:
	OpcSimd(){}
};

#endif
