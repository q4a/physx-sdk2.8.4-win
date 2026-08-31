#ifndef PXS_THRESHOLDTABLE_H
#define PXS_THRESHOLDTABLE_H

#include "PxcArray.h"
#include "PxcBitMap.h"
#include "PxcHashFunction.h"
#include "PxsSolverAtom.h"
#include "PxsBodyAtom.h"

template <class BodyPtr>
class PxcThresholdTable
{
public:

	static const int NO_INDEX = 0xffffffff;

	// only really needed to pre-size the hash, since the pair array is auto-resizing.

	void init(PxU32 bodyPairCount)
	{
		mPairs.reserve(bodyPairCount);
		mHash.clear();
		mHash.setSize(bodyPairCount*2+1);
		for(PxU32 i=0;i<mHash.getSize();i++)
			mHash[i] = 0xffffffff;
	}

	// called in last iteration of solver. Solver only needs to build force table.

	PxU64 ptrToPxU64(void *p) { return reinterpret_cast<PxU64>(p); }

	void addForce(BodyPtr body0, BodyPtr body1, PxFloat normalForce)
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
		Pair e = { body0, body1, 0xffffffff, normalForce };
		mPairs.insert(e);
	}

	template <class BodyPair>
	void getThresholdPairs(PxcArray<BodyPair> &accumulator)
	{
		for(PxU32 i=0;i<mPairs.getSize();i++)
		{
			PxFloat t0 = mPairs[i].body0 ? mPairs[i].body0->getReportThreshold() : PXD_FLOAT_MAX;
			PxFloat t1 = mPairs[i].body1 ? mPairs[i].body1->getReportThreshold() : PXD_FLOAT_MAX;

			BodyPair b = {  mPairs[i].body0, mPairs[i].body1  };

			if(mPairs[i].force>PxcMin<float>(t0,t1))
				accumulator.insert(b);
		}
	}


private:
	struct Pair
	{
		BodyPtr body0, body1;
		PxU32 next;					// next ptr for hash
		PxFloat force;
	};

	PxU32					mMaxPairCount;
	PxcArray<PxU32>			mHash;
	PxcArray<Pair>			mPairs;
};

#endif
