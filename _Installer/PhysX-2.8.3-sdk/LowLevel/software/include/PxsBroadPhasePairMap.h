#ifndef PXS_BROADPHASE_PAIRMAP_H
#define PXS_BROADPHASE_PAIRMAP_H

#define PXS_BP_PAIRBUCKET_SIZE 4


struct PxsBpPairBucket
{
	PxU16 mPairs[PXS_BP_PAIRBUCKET_SIZE];
	PxU16 mNextPairBucket;
	PxU16 mOwner;
};

class PxsBroadPhasePairIterator
{
public:
	bool getNext(PxU16 &volA, PxU16 &volB)
	{
		PxU32 idx = mIterator.getNext();
		if(idx == PxcBitMapIterator::DONE)
			return false;

		PxU32 bucketIdx, itemIdx;

		bitToBucketIndex(idx, bucketIdx, itemIdx);

		volA = mPairBuckets[bucketIdx].mOwner;
		volB = mPairBuckets[bucketIdx].mPairs[itemIdx];

		return true;
	}

	void reset()
	{
		mIterator.reset();
	}

	PxsBroadPhasePairIterator(PxcBitMap &bmap, PxcArray<PxsBpPairBucket> &buckets) :
	   mIterator(bmap),
	   mPairBuckets(buckets)
	{
	}

private:

	PXD_FORCE_INLINE void bitToBucketIndex(PxU32 bitIdx, PxU32 &bucketIndex, PxU32 &itemIdx)
	{
		bucketIndex = bitIdx / PXS_BP_PAIRBUCKET_SIZE;
		itemIdx = bitIdx % PXS_BP_PAIRBUCKET_SIZE;
	}

	PxcBitMapIterator mIterator;
	PxcArray<PxsBpPairBucket> &mPairBuckets;
};

//class PxsBroadPhasePairIterator;


template <typename Volume>
class PxsBroadPhasePairMapBase
{
public:

	PxsBroadPhasePairMapBase(PxcArray<Volume> &smallVolumes);
	~PxsBroadPhasePairMapBase();

	PxsBroadPhasePairIterator getCreatedPairs()
	{
		return PxsBroadPhasePairIterator(mCreatedPairMap, mPairBuckets);
	}

	PxsBroadPhasePairIterator getDestroyedPairs()
	{
		return PxsBroadPhasePairIterator(mDestroyedPairMap, mPairBuckets);
	}

	PxU32 getCreatedOverlapCount() const
	{
		return mCreatedPairMap.getSetBitCount();
	}

	PxU32 getDestroyedOverlapCount() const
	{
		return mDestroyedPairMap.getSetBitCount();
	}

	PxU32 getOverlapCount() const
	{
		return mActivePairMap.getSetBitCount();
	}

	void purgePairBuckets();
	void update();
	bool isConsistent();

	bool checkPair(PxU32 volA, PxU32 volB); //returns true if there is an active pair
	bool checkDuplicatePairs(PxU32 sv);	//returns true if there are no duplicates.

	void dumpPairMaps();
	PxU32 getPairBucketCount(PxU32 sv);
	
	PXD_FORCE_INLINE PxU32 getAllocatedPairCount(PxU32 sv)
	{
		return getPairBucketCount(sv) * PXS_BP_PAIRBUCKET_SIZE;
	}
	PxU32 getUsedPairCount(PxU32 sv);
	PxU32 getActivePairCount(PxU32 sv);

	//Note: purgeBpPair() does not schedule the pair bucket for delete, it assumes the caller will handle this
	//with deletePairBuckets().
	void purgeBpPair(PxU32 volA, PxU32 volB);
#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	void purgeAllBpPairs(PxU32 vol);
#endif
	void deletePairBuckets(PxU32 svIdx);

	//Normal pair create/destroy due to moving overlaps.
//	PXD_FORCE_INLINE
	void destroyBpPair(PxU32 volA, PxU32 volB);
	void createBpPair(PxU32 volA, PxU32 volB);

#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	virtual void purgeInvalidBpPairs(PxU32 vol, PxcBitMap& deleteVolumes);
#endif

protected:

	// Only an interface for the real implementation which should be provided by the subclasses
	// (Usually you would use polymorphism but want to avoid the additional indirection of virtual
	//  methods here)
	virtual bool purgeBpPairInBucket(PxU32 bucketIndex, PxU32 volB);
#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	virtual PxU32 purgeAllBpPairsInBucket(PxU32 bucketIndex);
#endif
	PXD_FORCE_INLINE bool destroyBpPairInBucket(PxU32 bucketIndex, PxU32 volB);
	PXD_FORCE_INLINE bool createBpPairInBucket(PxU32 bucketIndex, PxU32 volB, PxU32 &bucketCount, PxU32 &freeBucket);
	PXD_FORCE_INLINE PxU32 newPairBucket()
	{

		if(mFreePairBucket!=0xffFF)
		{
			PxU32 rv = mFreePairBucket;
			mFreePairBucket = mPairBuckets[rv].mNextPairBucket;
			return rv;
		}

		if(mPairBuckets.getSize() >= 0xffFF)
			return 0;

		mPairBuckets.insert();
		PxU32 newIdx = mPairBuckets.getSize()-1;

#ifdef _DEBUG
		memset(&(mPairBuckets[newIdx]), 0xcd, sizeof(PxsBpPairBucket));
#endif

		return newIdx;
	}

	PXD_FORCE_INLINE void deletePairBucket(PxU32 idx)
	{

#ifdef _DEBUG
		memset(&(mPairBuckets[idx]), 0xcd, sizeof(PxsBpPairBucket));
#endif

		mPairBuckets[idx].mNextPairBucket = mFreePairBucket;
		mFreePairBucket = idx;
	}

	PXD_FORCE_INLINE PxU32 bucketToBitIndex(PxU32 bucketIdx, PxU32 itemIdx)
	{
		PXN_ASSERT(itemIdx < PXS_BP_PAIRBUCKET_SIZE);
		return bucketIdx*PXS_BP_PAIRBUCKET_SIZE + itemIdx;
	}

	PXD_FORCE_INLINE void bitToBucketIndex(PxU32 bitIdx, PxU32 &bucketIndex, PxU32 &itemIdx)
	{
		bucketIndex = bitIdx / PXS_BP_PAIRBUCKET_SIZE;
		itemIdx = bitIdx % PXS_BP_PAIRBUCKET_SIZE;
	}

	PxcBitMap						mActivePairMap;
	PxcBitMap						mOldActivePairMap;
	PxcBitMap						mFreePairMap;
	//transient, use this rather than compares of 
	//mActivePairMap, mOldActivePairMap so we can use fast bitmap routines.
	//TODO: we can eliminate this with a clever iterator(iterate less bits/iterate greater bits)
	//but more work when retrieving overlaps.
	PxcBitMap						mCreatedPairMap; 
	PxcBitMap						mDestroyedPairMap; 
	PxcBitMap						mNewlyFreePairMap;

	//pairs
	PxcArray<PxsBpPairBucket>		mPairBuckets;
	PxU16							mFreePairBucket;

	//small volumes
	PxcArray<Volume>				&mSmallVolumes;

	bool							mExcessivePairError;
	
};

template <typename V>
PXD_FORCE_INLINE bool PxsBroadPhasePairMapBase<V>::destroyBpPairInBucket(PxU32 bucketIndex, PxU32 volB)
{
	while(bucketIndex!=0)
	{
#if PXS_BP_PAIRBUCKET_SIZE == 4

		PxU32 eqMask = 
			(mPairBuckets[bucketIndex].mPairs[0] == volB) |
			((mPairBuckets[bucketIndex].mPairs[1] == volB) << 1) |
			((mPairBuckets[bucketIndex].mPairs[2] == volB) << 2) |
			((mPairBuckets[bucketIndex].mPairs[3] == volB) << 3);

		PxU32 allocMask = ~ mFreePairMap.getNibbleFast(bucketIndex);
		PxU32 matchMask = eqMask & allocMask;
		
		PXN_ASSERT(PxcBitCount32(matchMask) <= 1);

		if(matchMask)
		{
			mActivePairMap.andNibbleFast(bucketIndex, ~matchMask);
			return true;
		}
#else
		for(PxU32 i=0; i<PXS_BP_PAIRBUCKET_SIZE; i++)
		{
			PxU32 bitIndex = bucketToBitIndex(bucketIndex, i);

			if(mFreePairMap.check(bitIndex))
			{
			}
			else if(mPairBuckets[bucketIndex].mPairs[i] == volB)
			{

				//deactivate pair
				mActivePairMap.clearFast(bitIndex);
				return true;
			}
		}
#endif

		bucketIndex = mPairBuckets[bucketIndex].mNextPairBucket;
	}
	return false;
}

template <typename V>
PXD_FORCE_INLINE bool PxsBroadPhasePairMapBase<V>::createBpPairInBucket(PxU32 bucketIndex, PxU32 volB, PxU32 &bucketCount, PxU32 &freeBucket)
{
	while(bucketIndex!=0)
	{
#if PXS_BP_PAIRBUCKET_SIZE == 4
		PxU32 eqMask = 
			(mPairBuckets[bucketIndex].mPairs[0] == volB) |
			((mPairBuckets[bucketIndex].mPairs[1] == volB) << 1) |
			((mPairBuckets[bucketIndex].mPairs[2] == volB) << 2) |
			((mPairBuckets[bucketIndex].mPairs[3] == volB) << 3);

		PxU32 freeMask = mFreePairMap.getNibbleFast(bucketIndex);
		PxU32 allocMask = ~freeMask;
		PxU32 matchMask = eqMask & allocMask;
		
		PXN_ASSERT(PxcBitCount32(matchMask) <= 1);

		if(matchMask)
		{
			mActivePairMap.orNibbleFast(bucketIndex, matchMask);
			return true;
		}
		else if(freeMask)
			freeBucket = bucketIndex;
#else
		for(PxU32 i=0; i<PXS_BP_PAIRBUCKET_SIZE; i++)
		{
			PxU32 bitIndex = bucketToBitIndex(bucketIndex, i);

			if(mFreePairMap.checkFast(bitIndex))
			{
				freeBucket = bucketIndex;
			}
			else if(mPairBuckets[bucketIndex].mPairs[i] == volB)
			{
				mActivePairMap.setFast(bitIndex);
				return true;
			}
			
		}
#endif

		bucketCount++;
		bucketIndex = mPairBuckets[bucketIndex].mNextPairBucket;
	}

	return false;
}



/*!
Pair map for the single SAP.
*/
class PxsBroadPhasePairMap : public PxsBroadPhasePairMapBase<PxsBpSmallVolume>
{
public:

	PxsBroadPhasePairMap(PxcArray<PxsBpSmallVolume> &smallVolumes);
	~PxsBroadPhasePairMap();
};


struct PxsBpSmallVolumeMulti;

/*!
Pair map for the multi SAP. Tracks pair information gathered from the SAP cells.
*/
class PxsBroadPhasePairMapMulti : public PxsBroadPhasePairMapBase<PxsBpSmallVolumeMulti>
{
public:

	PxsBroadPhasePairMapMulti(PxcArray<PxsBpSmallVolumeMulti> &smallVolumes);
	~PxsBroadPhasePairMapMulti();
};


/*!
Pair map for a cell of the multi SAP. Compared to the single SAP this pair map needs to track
the removed overlaps of removed volumes as well to propagate them to the top-level pair map.
*/
class PxsBroadPhasePairMapCell : public PxsBroadPhasePairMapBase<PxsBpSmallVolume>
{
public:

	PxsBroadPhasePairMapCell(PxcArray<PxsBpSmallVolume> &smallVolumes);
	~PxsBroadPhasePairMapCell();

	virtual bool purgeBpPairInBucket(PxU32 bucketIndex, PxU32 volB);
#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	virtual PxU32 purgeAllBpPairsInBucket(PxU32 bucketIndex);
	virtual void purgeInvalidBpPairs(PxU32 vol, PxcBitMap& deleteVolumes);
#endif

	PxsBroadPhasePairIterator getDeletedVolumePairs()
	{
		return PxsBroadPhasePairIterator(mDeletedVolumePairMap, mPairBuckets);
	}

	PxU32 getDeletedVolumePairsCount() const
	{
		return mDeletedVolumePairMap.getSetBitCount();
	}

	void clearDeletedVolumePairs()
	{
		mDeletedVolumePairMap.reset(mActivePairMap.getHighestSetBit());
	}

private:

	PxcBitMap						mDeletedVolumePairMap;
};


#endif

