#ifndef PXC_SIMD_WIN_H
#define PXC_SIMD_WIN_H

#include "PxnError.h"

#include "PxcPlatformInclude.h"
#include "PxcVector.h"
#include "PxcPlane.h"

#include "PxcCPUDetection.h"

/*!
Notice on aligned functions:
Functions ending with A is operating on aligned data. They expect the input parameters
to be 16-byte aligned. PxcVectors will be read/written as 4 component vectors!
*/

//! SSE version
class PxcSimd
{
public:
	typedef __m128 Vector4;

	static PXD_FORCE_INLINE bool isSupported()
	{
		static bool sseSupport = PxcCPUSupportSSE();
		return sseSupport;
	}

	/* Setup any rounding modes etc */
	static PXD_FORCE_INLINE PxU32 setup()
	{
		PxU32 oldMXCSR = _mm_getcsr();
		_mm_setcsr(oldMXCSR | 0x8040); // set DAZ and FZ bits
		return oldMXCSR;
	}

	/* Reset any setup rounding modes etc*/
	static PXD_FORCE_INLINE void reset(PxU32 fromSetup)
	{
		_mm_setcsr(fromSetup);
	}

	/* loads */	
	static PXD_FORCE_INLINE Vector4 load(const PxcVector &pxcVec)
	{
		return _mm_loadu_ps(&pxcVec.x);
	}

	//A = aligned
	static PXD_FORCE_INLINE Vector4 loadA(const PxcVector &pxcVec)
	{
		return _mm_load_ps(&pxcVec.x);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxFloat &a)
	{
		return _mm_load_ss(&a);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU32 &a)
	{
		return _mm_load_ss((PxFloat*)&a);
	}

	static PXD_FORCE_INLINE Vector4 load(const PxU8 &a)
	{
		PxU32 u = (PxU32)a;
		return _mm_load_ss((PxFloat*)&u);
	}

	/* stores */
	static PXD_FORCE_INLINE void store(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		//Ugly, cannot write outside pxcVec
		_mm_store_ss(&pxcVec.x, simdVec); //store x
		_mm_store_ss(&pxcVec.y, _mm_shuffle_ps(simdVec, simdVec, _MM_SHUFFLE(1,1,1,1))); //store z
		_mm_store_ss(&pxcVec.z, _mm_shuffle_ps(simdVec, simdVec, _MM_SHUFFLE(2,2,2,2))); //store z
	}

	//A = aligned
	static PXD_FORCE_INLINE void storeA(PxcVector &pxcVec, const Vector4 &simdVec)
	{
		_mm_store_ps(&pxcVec.x, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxFloat &a, const Vector4 &simdVec)
	{
		_mm_store_ss(&a, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxU32 &a, const Vector4 &simdVec)
	{
		_mm_store_ss((PxFloat*)&a, simdVec);
	}

	static PXD_FORCE_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		PxU32 u;
		_mm_store_ss((PxFloat*)&u, simdVec);
		a = u;
	}

	/* basic math */
	static PXD_FORCE_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return _mm_add_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return _mm_sub_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return _mm_mul_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 multiplyAdd(const Vector4 &a, const Vector4 &b, const Vector4 &c)
	{
		return _mm_add_ps(_mm_mul_ps(a, b),c);
	}

	static PXD_FORCE_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return _mm_rcp_ps(a);
	}

	static PXD_FORCE_INLINE Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return _mm_rsqrt_ps(a);
	}

	// 3 component dot product
	static PXD_FORCE_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		__m128 dot1 = _mm_mul_ps(a, b);
		__m128 shuf1 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(3,0,2,1));
		__m128 shuf2 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(3,1,0,2));

		return _mm_add_ps(shuf2, _mm_add_ps(dot1,shuf1));
	}

	static PXD_FORCE_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		__m128 dot1 = _mm_mul_ps(a, b);										//x,y,z,w
		__m128 shuf1 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(2,1,0,3));	//w,x,y,z
		__m128 shuf2 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(1,0,3,2));	//z,w,x,y
		__m128 shuf3 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(0,3,2,1));	//y,z,w,x

		return _mm_add_ps(_mm_add_ps(shuf2, shuf3), _mm_add_ps(dot1,shuf1));
	}

	static PXD_FORCE_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		__m128 l1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)); //y,z,x,w
		__m128 l2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)); //z,x,y,w

		__m128 r1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)); //z,x,y,w
		__m128 r2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)); //y,z,x,w

		return _mm_sub_ps(_mm_mul_ps(l1, l2), _mm_mul_ps(r1,r2));
	}

	static PXD_FORCE_INLINE Vector4 abs(const Vector4 &a)
	{
		__declspec(align(16)) const static PxU32 absMask[4] = {0x7fFFffFF, 0x7fFFffFF, 0x7fFFffFF, 0x7fFFffFF};
		return _mm_and_ps(a, _mm_load_ps((PxFloat*)absMask));
		//return _mm_max_ps(a, _mm_sub_ps(_mm_setzero_ps(), a));
	}

	static PXD_FORCE_INLINE Vector4 zero()
	{
		return _mm_setzero_ps();
	}

	static PXD_FORCE_INLINE Vector4 one()
	{
		return _mm_set_ps1(1.0f);
	}

	static PXD_FORCE_INLINE Vector4 allSet()
	{
		Vector4 a = zero();
		return equal(a,a);
	}

	static PXD_FORCE_INLINE Vector4 eps()
	{
		return _mm_set_ps1(PXD_FLOAT_EPSILON);
	}

	static PXD_FORCE_INLINE Vector4 eps6()
	{
		return _mm_set_ps1(1e-6f);
	}

	/* Min/max , per component */
	static PXD_FORCE_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return _mm_max_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return _mm_min_ps(a, b);
	}


	/* splatting */
	static PXD_FORCE_INLINE Vector4 splatX(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(0,0,0,0));
	}

	static PXD_FORCE_INLINE Vector4 splatY(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(1,1,1,1));
	}

	static PXD_FORCE_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(2,2,2,2));
	}

	static PXD_FORCE_INLINE Vector4 splatW(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(3,3,3,3));
	}

	/* permutations and selects */
	static PXD_FORCE_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		__m128 vxy = _mm_move_ss(vY, vX);							//vX.x , vY.y, vY.z, vY.w;
		return _mm_shuffle_ps(vxy, vZ, _MM_SHUFFLE(3,2,1,0));		//vxy.X, vxy.Y, vZ.z, vZ.w
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static PXD_FORCE_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(0,0,0,0));		//vX.x, vX.x, vY.x, vY.x
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(0,0,2,0));	//vX.x, vY.x, vZ.x, vZ.x
		return vxy;
	}

	static PXD_FORCE_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(1,1,1,1));		//vX.y, vX.y, vY.y, vY.y
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(1,1,2,0));	//vX.y, vY.y, vZ.y, vZ.y
		return vxy;
	}

	static PXD_FORCE_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(2,2,2,2));		//vX.z, vX.z, vY.z, vY.z
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(2,2,2,0));	//vX.z, vY.z, vZ.z, vZ.z
		return vxy;
	}


	static PXD_FORCE_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		//TODO RETHINK
		ASSERT(!"Not implemented!");	
		Vector4 result;
		return result;
	}

	static PXD_FORCE_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return _mm_or_ps(_mm_andnot_ps(control, a), _mm_and_ps(control, b));
	}


	/* float comparisons (bitmasks)*/
	static PXD_FORCE_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpeq_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpneq_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmplt_ps(a,b);
	}

	static PXD_FORCE_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpgt_ps(a,b);
	}

	static PXD_FORCE_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmple_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpge_ps(a,b);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static PXD_FORCE_INLINE PxIBool equalBool(const Vector4 &a, const Vector4 &b)
	{	
		__m128 mask = equal(a,b);
		return allBitsSet(mask);
	}

	static PXD_FORCE_INLINE PxIBool notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = notEqual(a,b);
		return allBitsSet(mask);
	}

	static PXD_FORCE_INLINE PxIBool lessBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = less(a,b);
		return allBitsSet(mask);
	}

	static PXD_FORCE_INLINE PxIBool greaterBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = greater(a,b);
		return allBitsSet(mask);
	}

	static PXD_FORCE_INLINE PxIBool lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = lessEqual(a,b);
		return allBitsSet(mask);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = greaterEqual(a,b);
		return allBitsSet(mask);
	}
	/* float comparisons bool, X component only */
	static PXD_FORCE_INLINE PxIBool equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comieq_ss(a, b);
	}

	static PXD_FORCE_INLINE PxIBool notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comineq_ss(a, b);
	}

	static PXD_FORCE_INLINE PxIBool lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comilt_ss(a, b);
	}

	static PXD_FORCE_INLINE PxIBool greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comigt_ss(a, b);
	}

	static PXD_FORCE_INLINE PxIBool lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comile_ss(a, b);
	}

	static PXD_FORCE_INLINE PxIBool greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comige_ss(a, b);
	}

	/* int comparisons (bitmasks) */
	static PXD_FORCE_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		//TODO: This have to be done in integer registers for now.. find a solution!
		__m128 mask = _mm_andnot_ps(a, b);
		__declspec(align(16)) PxU32 intRegs[4];
		_mm_store_ps((PxFloat*)&intRegs[0], mask);

		if(intRegs[0] != 0) intRegs[0] = 0xFFffFFff;
		if(intRegs[1] != 0) intRegs[1] = 0xFFffFFff;
		if(intRegs[2] != 0) intRegs[2] = 0xFFffFFff;
		if(intRegs[3] != 0) intRegs[3] = 0xFFffFFff;
		
		return _mm_load_ps((PxFloat*)&intRegs[0]);
	}

	/* int comparisons bool (first 3 components)*/
	static PXD_FORCE_INLINE PxIBool intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = intNotEqual(a,b);
		return allBitsSet(mask);// wrong(should be anyBitsSet)
	}

	/* bitwise logical operations */
	static PXD_FORCE_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return _mm_and_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return _mm_or_ps(a, b);
	}

	static PXD_FORCE_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return _mm_andnot_ps(b, a);
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

private:

	//Helper
	static PXD_FORCE_INLINE PxIBool allBitsSet(const Vector4& mask)
	{
		//and them together
		__m128 shuf1 = _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(3,0,2,1));	//y, z, x, w
		__m128 shuf2 = _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(3,1,0,2));	//z, x, y, w

		__m128 finalMask = _mm_and_ps(mask, _mm_and_ps(shuf1, shuf2));
		__m128 eq = _mm_andnot_ps(finalMask, _mm_set_ps1(1.0f));

		return _mm_ucomieq_ss(eq, _mm_setzero_ps());
	}

	PxcSimd(){}
};


#endif
