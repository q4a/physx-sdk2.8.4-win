#ifndef PXS_BASETHRESHOLDTABLE_H
#define PXS_BASETHRESHOLDTABLE_H

#include "PxcArray.h"
#include "PxcBitMap.h"
#include "PxcHashFunction.h"
#include "PxsSolverAtom.h"
#include "PxsBodyAtom.h"

template <class BodyPtr>
class PxcBaseThresholdTable
{
private:

	static const int NO_INDEX = 0xffffffff;

	struct Pair
	{
		BodyPtr body0, body1;
		PxU32 next;		// hash key & next ptr
		PxFloat force;
		PxFloat threshold;
	};

	PxU32					mMaxPairCount;
	PxcArray<PxU32>			mHash;
	PxcArray<Pair>			mPairs;

	PxU64 ptrToPxU64(void *p)
	{
#if defined(PXD_PLATFORM_32) || defined(PXD_PLATFORM_XBOX360)
		return (PxU64)(PxU32)(p);
#else
		return (PxU64)(p);
#endif
	}

protected:

	void initTable(PxU32 bodyPairCount)
	{
		mPairs.reserve(bodyPairCount);
		mPairs.clear();
		mHash.clear();
		mHash.setSize(bodyPairCount*2+1);
		for(PxU32 i=0;i<mHash.getSize();i++)
			mHash[i] = 0xffffffff;
	}

	// called in last iteration of solver. Solver only needs to build force table.

	void addForceInternal(BodyPtr body0, BodyPtr body1, PxFloat normalForce, PxFloat threshold)
	{
		PxcSort(body0,body1);
		PxU32 mKey = PxcHashFunction(ptrToPxU64(body0)<<32 | ptrToPxU64(body1)) % mHash.getSize();

		PxU32 *pairIndex = &mHash[mKey];
		while(*pairIndex!=0xffffffff)
		{
			Pair &e = mPairs[*pairIndex];
			if(e.body0==body0 && e.body1==body1)
			{	
				e.force += normalForce;
				return;
			}
			pairIndex = &e.next;
		}

		*pairIndex = mPairs.getSize();
		Pair e = { body0, body1, 0xffffffff, normalForce, threshold};
		mPairs.insert(e);
	}

	template <class BodyPair>
	void getThresholdPairsInternal(PxcArray<BodyPair> &accumulator)
	{

		for(PxU32 i=0;i<mPairs.getSize();i++)
		{
			Pair &p = mPairs[i];
			if(mPairs[i].force > p.threshold)
			{
				BodyPair b = {p.body0, p.body1};
				accumulator.insert(b);
			}
		}

	}

};

#endif
