#ifndef PXC_PROJECTION_H
#define PXC_PROJECTION_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcHelperFunction.h"
#include "PxcSimd.h"

// A bunch of function for quickly projecting a set of points along a direction and 
// finding the min and max etc.

#if !defined(PXD_SUPPORT_SIMD)

class PxcProjection
{
public: 

	// The (approximate) number of vertices for which it is faster to use brute force projection
	// rather than something smarter(eg hill climbing).
	static const PxUInt BRUTE_THRESHOLD = 32;

	//note: transform is the local=>world transform and dir is in world.
	// also adds the translation from transform
	static PXD_FORCE_INLINE void pointsOnDir(const PxcVector& dir, const PxcMatrix34& transform, 
		const PxcVector* PXD_RESTRICT verts, PxUInt numVerts, PxFloat& min, PxFloat& max)
	{
		PxcVector localDir = transform.rotateTranspose(dir);

		pointsOnDir(localDir, verts, numVerts, min, max);

		PxFloat offset = transform.base3.dot(dir);
		min += offset;
		max += offset;

		PXN_ASSERT(min <= max);
	}

	static PXD_FORCE_INLINE void pointsOnDir(const PxcVector& dir, const PxcVector* verts, PxUInt numVerts, PxFloat& _min, PxFloat& _max)
	{
		// PT: prevents aliasing
		PxFloat min = PXD_FLOAT_MAX;
		PxFloat max = -PXD_FLOAT_MAX;

		while(numVerts--)
		{
			PxFloat dp = (*verts++).dot(dir);
			if(dp < min) min = dp;
			if(dp > max) max = dp;
		}

		_min = min;
		_max = max;

		// PT: eeeerr? what's the point of this line?
//		if(min>max) PxcSwap(min, max);
	}

private:
	PxcProjection(){}
};

#else

class PxcProjection
{
public:

	// The (approximate) number of vertices for which it is faster to use brute force projection
	// rather than something smarter(eg hill climbing).
	// Note: in terms of instructions we execute ~twice as many but it takes much less time due to
	// better pipelining. (at least up to ~100)
	static const PxUInt BRUTE_THRESHOLD = 128;

	//note: transform is the local=>world transform and dir is in world.
	// we need to do the rotation with simd to avoid LHSs
	// also adds the translation from transform
	static PXD_FORCE_INLINE void pointsOnDir(const PxcVector& dir, const PxcMatrix34& transform, 
		const PxcVector* PXD_RESTRICT verts, PxUInt numVerts, PxFloat& min, PxFloat& max)
	{
		PxcSimd::Vector4 dir4 = PxcSimd::load(dir);
		PxcSimd::Vector4 basis0 = PxcSimd::load(transform.base0);
		PxcSimd::Vector4 basis1 = PxcSimd::load(transform.base1);
		PxcSimd::Vector4 basis2 = PxcSimd::load(transform.base2);
		PxcSimd::Vector4 basis3 = PxcSimd::load(transform.base3);

		PxcSimd::Vector4 localDir4 = PxcSimd::rotateInv(basis0, basis1, basis2, dir4);

		PxcSimd::Vector4 min4, max4;

		pointsOnDir(localDir4, verts, numVerts, min4, max4);

		PxcSimd::Vector4 offset4 = PxcSimd::dot(basis3, dir4);
		min4 = PxcSimd::add(min4, offset4);
		max4 = PxcSimd::add(max4, offset4);

		PxcSimd::store(min, min4);
		PxcSimd::store(max, max4);

		PXN_ASSERT(min <= max);
	}

	static PXD_FORCE_INLINE void pointsOnDir(const PxcVector& dir, const PxcVector* PXD_RESTRICT verts, PxUInt numVerts,
								PxFloat& min, PxFloat& max)
	{
		PxcSimd::Vector4 dir4 = PxcSimd::load(dir);
		PxcSimd::Vector4 min4, max4;

		pointsOnDir(dir4, verts, numVerts, min4, max4);

		PxcSimd::store(min, min4);
		PxcSimd::store(max, max4);

		PXN_ASSERT(min <= max);

	}

	static PXD_FORCE_INLINE void pointsOnDir(const PxcSimd::Vector4& dir4, const PxcVector* PXD_RESTRICT verts, PxUInt numVerts,
								PxcSimd::Vector4& min4, PxcSimd::Vector4& max4)
	{
		//split to eliminate dependance within loop.
		PxcSimd::Vector4 min4_1 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_1 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_2 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_2 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_3 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_3 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_4 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_4 = PxcSimd::floatMin();

		PxUInt numRolled = numVerts & 0x3;
		
		for(PxUInt i=0; i<numRolled; i++)
		{
			PxcSimd::Vector4 dp = PxcSimd::dot(dir4, PxcSimd::load(*verts));

			min4_1 = PxcSimd::min(min4_1, dp);
			max4_1 = PxcSimd::max(max4_1, dp);

			verts++;
		}

		PxUInt numUnRolled = numVerts >> 2;

		for(PxUInt i=0; i<numUnRolled; i++)
		{
			PxcSimd::Vector4 dp_1 = PxcSimd::dot(dir4, PxcSimd::load(verts[0]));
			min4_1 = PxcSimd::min(min4_1, dp_1);
			max4_1 = PxcSimd::max(max4_1, dp_1);

			PxcSimd::Vector4 dp_2 = PxcSimd::dot(dir4, PxcSimd::load(verts[1]));
			min4_2 = PxcSimd::min(min4_2, dp_2);
			max4_2 = PxcSimd::max(max4_2, dp_2);

			PxcSimd::Vector4 dp_3 = PxcSimd::dot(dir4, PxcSimd::load(verts[2]));
			min4_3 = PxcSimd::min(min4_3, dp_3);
			max4_3 = PxcSimd::max(max4_3, dp_3);

			PxcSimd::Vector4 dp_4 = PxcSimd::dot(dir4, PxcSimd::load(verts[3]));
			min4_4 = PxcSimd::min(min4_4, dp_4);
			max4_4 = PxcSimd::max(max4_4, dp_4);

			verts+=4;
		}

		PxcSimd::Vector4 tmp1 = PxcSimd::min(min4_1, min4_2);
		PxcSimd::Vector4 tmp2 = PxcSimd::min(min4_3, min4_4);
		min4 = PxcSimd::min(tmp1, tmp2);

		PxcSimd::Vector4 tmp3 = PxcSimd::max(max4_1, max4_2);
		PxcSimd::Vector4 tmp4 = PxcSimd::max(max4_3, max4_4);
		max4 = PxcSimd::max(tmp3, tmp4);

	}

	static PXD_FORCE_INLINE void pointsOnDirA(const PxcSimd::Vector4& dir4, const PxcVector* PXD_RESTRICT verts, PxUInt numVerts,
								PxcSimd::Vector4& min4, PxcSimd::Vector4& max4)
	{
		//check alignment
		PXN_ASSERT( ((PxU32)verts & 0xf) == 0);

		//split to eliminate dependance within loop.
		PxcSimd::Vector4 min4_1 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_1 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_2 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_2 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_3 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_3 = PxcSimd::floatMin();
		PxcSimd::Vector4 min4_4 = PxcSimd::floatMax();
		PxcSimd::Vector4 max4_4 = PxcSimd::floatMin();

		const PxcSimd::Vector4 *ptr = (PxcSimd::Vector4 *)verts;
		const PxcSimd::Vector4 *endPtr = (PxcSimd::Vector4 *)(verts + (numVerts & ~0x3));
		
		while(ptr<endPtr)
		{
			PxcSimd::Vector4 d1 = PxcSimd::loadA(ptr[0]);
			PxcSimd::Vector4 d2 = PxcSimd::loadA(ptr[1]);
			PxcSimd::Vector4 d3 = PxcSimd::loadA(ptr[2]);

			PxcSimd::Vector4 v1, v2, v3, v4;
			
			PxcSimd::unpack3to4(d1, d2, d3, v1, v2, v3, v4);

			PxcSimd::Vector4 dp_1 = PxcSimd::dot(dir4, v1);
			min4_1 = PxcSimd::min(min4_1, dp_1);
			max4_1 = PxcSimd::max(max4_1, dp_1);

			PxcSimd::Vector4 dp_2 = PxcSimd::dot(dir4, v2);
			min4_2 = PxcSimd::min(min4_2, dp_2);
			max4_2 = PxcSimd::max(max4_2, dp_2);

			PxcSimd::Vector4 dp_3 = PxcSimd::dot(dir4, v3);
			min4_3 = PxcSimd::min(min4_3, dp_3);
			max4_3 = PxcSimd::max(max4_3, dp_3);

			PxcSimd::Vector4 dp_4 = PxcSimd::dot(dir4, v4);
			min4_4 = PxcSimd::min(min4_4, dp_4);
			max4_4 = PxcSimd::max(max4_4, dp_4);

			ptr+=3;
		}

		const PxcVector* PXD_RESTRICT vertsPtr = verts + (numVerts & ~0x3);
		const PxcVector* PXD_RESTRICT vertsPtrEnd = verts + numVerts;

		while(vertsPtr < vertsPtrEnd)
		{
			PxcSimd::Vector4 dp = PxcSimd::dot(dir4, PxcSimd::load(*vertsPtr));

			min4_1 = PxcSimd::min(min4_1, dp);
			max4_1 = PxcSimd::max(max4_1, dp);

			vertsPtr++;
		}

		PxcSimd::Vector4 tmp1 = PxcSimd::min(min4_1, min4_2);
		PxcSimd::Vector4 tmp2 = PxcSimd::min(min4_3, min4_4);
		min4 = PxcSimd::min(tmp1, tmp2);

		PxcSimd::Vector4 tmp3 = PxcSimd::max(max4_1, max4_2);
		PxcSimd::Vector4 tmp4 = PxcSimd::max(max4_3, max4_4);
		max4 = PxcSimd::max(tmp3, tmp4);
	}

private:
	PxcProjection(){}
};

#endif

#endif
