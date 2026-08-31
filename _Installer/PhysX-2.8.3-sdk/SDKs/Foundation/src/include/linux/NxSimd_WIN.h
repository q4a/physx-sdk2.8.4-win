#ifndef NX_SIMD_WIN_H
#define NX_SIMD_WIN_H

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

//! SSE version
class NxSimd
{
public:
	typedef __m128 Vector4;

	static NX_INLINE bool isSupported()
	{
		//static bool sseSupport = PxcCPUSupportSSE();
		//return sseSupport;

		NX_ASSERT(!"CPU detection not supported by NxSimd");
		return true;
	}

	/* Setup any rounding modes etc */
	static NX_INLINE NxU32 setup()
	{
		NxU32 oldMXCSR = _mm_getcsr();
		_mm_setcsr(oldMXCSR | 0x8040); // set DAZ and FZ bits
		return oldMXCSR;
	}

	/* Reset any setup rounding modes etc*/
	static NX_INLINE void reset(NxU32 fromSetup)
	{
		_mm_setcsr(fromSetup);
	}

	/* loads */	
	static NX_INLINE Vector4 load(const NxVec3 &pxcVec)
	{
		return _mm_loadu_ps(&pxcVec.x);
	}

	//A = aligned
	static NX_INLINE Vector4 loadA(const NxVec3 &pxcVec)
	{
		return _mm_load_ps(&pxcVec.x);
	}

	static NX_INLINE Vector4 load(const NxReal &a)
	{
		return _mm_load_ss(&a);
	}

	static NX_INLINE Vector4 load(const NxU32 &a)
	{
		return _mm_load_ss((NxReal*)&a);
	}

	static NX_INLINE Vector4 load(const PxU8 &a)
	{
		NxU32 u = (NxU32)a;
		return _mm_load_ss((NxReal*)&u);
	}

	/* stores */
	static NX_INLINE void store(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		//Ugly, cannot write outside pxcVec
		_mm_store_ss(&pxcVec.x, simdVec); //store x
		_mm_store_ss(&pxcVec.y, _mm_shuffle_ps(simdVec, simdVec, _MM_SHUFFLE(1,1,1,1))); //store z
		_mm_store_ss(&pxcVec.z, _mm_shuffle_ps(simdVec, simdVec, _MM_SHUFFLE(2,2,2,2))); //store z
	}

	//A = aligned
	static NX_INLINE void storeA(NxVec3 &pxcVec, const Vector4 &simdVec)
	{
		_mm_store_ps(&pxcVec.x, simdVec);
	}

	static NX_INLINE void store(NxReal &a, const Vector4 &simdVec)
	{
		_mm_store_ss(&a, simdVec);
	}

	static NX_INLINE void store(NxU32 &a, const Vector4 &simdVec)
	{
		_mm_store_ss((NxReal*)&a, simdVec);
	}

	static NX_INLINE void store(PxU8 &a, const Vector4 &simdVec)
	{
		NxU32 u;
		_mm_store_ss((NxReal*)&u, simdVec);
		a = u;
	}

	/* basic math */
	static NX_INLINE Vector4 add(const Vector4 &a, const Vector4 &b)
	{
		return _mm_add_ps(a, b);
	}

	static NX_INLINE Vector4 subtract(const Vector4 &a, const Vector4 &b)
	{
		return _mm_sub_ps(a, b);
	}

	static NX_INLINE Vector4 multiply(const Vector4 &a, const Vector4 &b)
	{
		return _mm_mul_ps(a, b);
	}

	static NX_INLINE Vector4 reciprocal(const Vector4 &a)
	{
		return _mm_rcp_ps(a);
	}

	static NX_INLINE Vector4 reciprocalSqrt(const Vector4 &a)
	{
		return _mm_rsqrt_ps(a);
	}

	// 3 component dot product
	static NX_INLINE Vector4 dot(const Vector4 &a, const Vector4 &b)
	{
		__m128 dot1 = _mm_mul_ps(a, b);
		__m128 shuf1 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(3,0,2,1));
		__m128 shuf2 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(3,1,0,2));

		return _mm_add_ps(shuf2, _mm_add_ps(dot1,shuf1));
	}

	static NX_INLINE Vector4 dot4(const Vector4 &a, const Vector4 &b)
	{
		__m128 dot1 = _mm_mul_ps(a, b);										//x,y,z,w
		__m128 shuf1 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(2,1,0,3));	//w,x,y,z
		__m128 shuf2 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(1,0,3,2));	//z,w,x,y
		__m128 shuf3 = _mm_shuffle_ps(dot1, dot1, _MM_SHUFFLE(0,3,2,1));	//y,z,w,x

		return _mm_add_ps(_mm_add_ps(shuf2, shuf3), _mm_add_ps(dot1,shuf1));
	}

	static NX_INLINE Vector4 cross(const Vector4 &a, const Vector4 &b)
	{
		__m128 l1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)); //y,z,x,w
		__m128 l2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)); //z,x,y,w

		__m128 r1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)); //z,x,y,w
		__m128 r2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)); //y,z,x,w

		return _mm_sub_ps(_mm_mul_ps(l1, l2), _mm_mul_ps(r1,r2));
	}

	static NX_INLINE Vector4 abs(const Vector4 &a)
	{
		__declspec(align(16)) const static NxU32 absMask[4] = {0x7fFFffFF, 0x7fFFffFF, 0x7fFFffFF, 0x7fFFffFF};
		return _mm_and_ps(a, _mm_load_ps((NxReal*)absMask));
		//return _mm_max_ps(a, _mm_sub_ps(_mm_setzero_ps(), a));
	}

	static NX_INLINE Vector4 zero()
	{
		return _mm_setzero_ps();
	}

	static NX_INLINE Vector4 one()
	{
		return _mm_set_ps1(1.0f);
	}

	static NX_INLINE Vector4 allSet()
	{
		Vector4 a = zero();
		return equal(a,a);
	}

	static NX_INLINE Vector4 eps()
	{
		return _mm_set_ps1(PXD_FLOAT_EPSILON);
	}

	static NX_INLINE Vector4 eps6()
	{
		return _mm_set_ps1(1e-6f);
	}

	/* Min/max , per component */
	static NX_INLINE Vector4 max(const Vector4 &a, const Vector4 &b)
	{
		return _mm_max_ps(a, b);
	}

	static NX_INLINE Vector4 min(const Vector4 &a, const Vector4 &b)
	{
		return _mm_min_ps(a, b);
	}


	/* splatting */
	static NX_INLINE Vector4 splatX(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(0,0,0,0));
	}

	static NX_INLINE Vector4 splatY(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(1,1,1,1));
	}

	static NX_INLINE Vector4 splatZ(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(2,2,2,2));
	}

	static NX_INLINE Vector4 splatW(const Vector4 &a)
	{
		return _mm_shuffle_ps(a, a, _MM_SHUFFLE(3,3,3,3));
	}

	/* permutations and selects */
	static NX_INLINE Vector4 mergeXYZ(const Vector4 &vX, const Vector4 &vY, const Vector4 &vZ)
	{
		__m128 vxy = _mm_move_ss(vY, vX);							//vX.x , vY.y, vY.z, vY.w;
		return _mm_shuffle_ps(vxy, vZ, _MM_SHUFFLE(3,2,1,0));		//vxy.X, vxy.Y, vZ.z, vZ.w
	}

	/* create a Vector4 from the x elements of 3 vectors(w undefined) */
	static NX_INLINE Vector4 columnX(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(0,0,0,0));		//vX.x, vX.x, vY.x, vY.x
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(0,0,2,0));	//vX.x, vY.x, vZ.x, vZ.x
		return vxy;
	}

	static NX_INLINE Vector4 columnY(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(1,1,1,1));		//vX.y, vX.y, vY.y, vY.y
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(1,1,2,0));	//vX.y, vY.y, vZ.y, vZ.y
		return vxy;
	}

	static NX_INLINE Vector4 columnZ(const Vector4 &vX,const Vector4 &vY,const Vector4 &vZ)
	{
		__m128 vxy1 = _mm_shuffle_ps(vX, vY, _MM_SHUFFLE(2,2,2,2));		//vX.z, vX.z, vY.z, vY.z
		__m128 vxy = _mm_shuffle_ps(vxy1, vZ, _MM_SHUFFLE(2,2,2,0));	//vX.z, vY.z, vZ.z, vZ.z
		return vxy;
	}


	static NX_INLINE Vector4 permute(const Vector4 &a, const Vector4 &b, 
		const PxUInt e0,const PxUInt e1,const PxUInt e2,const PxUInt e3)
	{
		//TODO RETHINK
		ASSERT(!"Not implemented!");	
		Vector4 result;
		return result;
	}

	static NX_INLINE Vector4 select(const Vector4 &a, const Vector4 &b, const Vector4 &control)
	{
		return _mm_or_ps(_mm_andnot_ps(control, a), _mm_and_ps(control, b));
	}


	/* float comparisons (bitmasks)*/
	static NX_INLINE Vector4 equal(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpeq_ps(a, b);
	}

	static NX_INLINE Vector4 notEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpneq_ps(a, b);
	}

	static NX_INLINE Vector4 less(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmplt_ps(a,b);
	}

	static NX_INLINE Vector4 greater(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpgt_ps(a,b);
	}

	static NX_INLINE Vector4 lessEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmple_ps(a, b);
	}

	static NX_INLINE Vector4 greaterEqual(const Vector4 &a, const Vector4 &b)
	{
		return _mm_cmpge_ps(a,b);
	}

	/* float comparisons (bool, for jumps)- first 3 components */
	/* Tests if _all_ components(3) are equal etc */
	static NX_INLINE NX_BOOL equalBool(const Vector4 &a, const Vector4 &b)
	{	
		__m128 mask = equal(a,b);
		return allBitsSet(mask);
	}

	static NX_INLINE NX_BOOL notEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = notEqual(a,b);
		return allBitsSet(mask);
	}

	static NX_INLINE NX_BOOL lessBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = less(a,b);
		return allBitsSet(mask);
	}

	static NX_INLINE NX_BOOL greaterBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = greater(a,b);
		return allBitsSet(mask);
	}

	static NX_INLINE NX_BOOL lessEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = lessEqual(a,b);
		return allBitsSet(mask);
	}

	static NX_INLINE NX_BOOL greaterEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = greaterEqual(a,b);
		return allBitsSet(mask);
	}
	/* float comparisons bool, X component only */
	static NX_INLINE NX_BOOL equalXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comieq_ss(a, b);
	}

	static NX_INLINE NX_BOOL notEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comineq_ss(a, b);
	}

	static NX_INLINE NX_BOOL lessXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comilt_ss(a, b);
	}

	static NX_INLINE NX_BOOL greaterXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comigt_ss(a, b);
	}

	static NX_INLINE NX_BOOL lessEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comile_ss(a, b);
	}

	static NX_INLINE NX_BOOL greaterEqualXBool(const Vector4 &a, const Vector4 &b)
	{
		return _mm_comige_ss(a, b);
	}

	/* int comparisons (bitmasks) */
	static NX_INLINE Vector4 intNotEqual(const Vector4 &a, const Vector4 &b)
	{
		//TODO: This have to be done in integer registers for now.. find a solution!
		__m128 mask = _mm_andnot_ps(a, b);
		__declspec(align(16)) NxU32 intRegs[4];
		_mm_store_ps((NxReal*)&intRegs[0], mask);

		if(intRegs[0] != 0) intRegs[0] = 0xFFffFFff;
		if(intRegs[1] != 0) intRegs[1] = 0xFFffFFff;
		if(intRegs[2] != 0) intRegs[2] = 0xFFffFFff;
		if(intRegs[3] != 0) intRegs[3] = 0xFFffFFff;
		
		return _mm_load_ps((NxReal*)&intRegs[0]);
	}

	/* int comparisons bool (first 3 components)*/
	static NX_INLINE NX_BOOL intNotEqualBool(const Vector4 &a, const Vector4 &b)
	{
		__m128 mask = intNotEqual(a,b);
		return allBitsSet(mask);//This is wrong! (should be anyBitsSet)
	}

	/* bitwise logical operations */
	static NX_INLINE Vector4 and4(const Vector4 &a, const Vector4 &b)
	{
		return _mm_and_ps(a, b);
	}

	static NX_INLINE Vector4 or4(const Vector4 &a, const Vector4 &b)
	{
		return _mm_or_ps(a, b);
	}

	static NX_INLINE Vector4 andNot(const Vector4 &a, const Vector4 &b)
	{
		return _mm_andnot_ps(b, a);
	}

private:

	//Helper
	static NX_INLINE NX_BOOL allBitsSet(const Vector4& mask)
	{
		//and them together
		__m128 shuf1 = _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(3,0,2,1));	//y, z, x, w
		__m128 shuf2 = _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(3,1,0,2));	//z, x, y, w

		__m128 finalMask = _mm_and_ps(mask, _mm_and_ps(shuf1, shuf2));
		__m128 eq = _mm_andnot_ps(finalMask, _mm_set_ps1(1.0f));

		return _mm_ucomieq_ss(eq, _mm_setzero_ps());
	}

	NxSimd(){}
};


#endif
