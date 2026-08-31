#ifndef PXS_BROADPHASEPAIRHOLDER_H
#define PXS_BROADPHASEPAIRHOLDER_H

#include "px_config.h"
#include "PxnError.h"
#include "PxnMemory.h"
#include "PxcHashFunction.h"

class PxsBroadPhaseOverlap
{
public:

	PXD_FORCE_INLINE PxsBroadPhaseOverlap() 
	: mVolumes(0)
	{
	}

	explicit PXD_FORCE_INLINE PxsBroadPhaseOverlap(PxU32 volumes) 
	: mVolumes(volumes)
	{
	}

	PXD_FORCE_INLINE PxsBroadPhaseOverlap(PxU32 volume0, PxU32 volume1)
	{
		// Make sure both are at most 16 bits
		PXN_ASSERT((0xFFFF & volume0) == volume0);
		PXN_ASSERT((0xFFFF & volume1) == volume1);
		if (volume0 > volume1)
		{
			// swap
			mVolumes = (0xFFFF & volume1) << 16 | (0xFFFF & volume0);
		}
		else
		{
			mVolumes = (0xFFFF & volume0) << 16 | (0xFFFF & volume1);
		}
	}

	PXD_FORCE_INLINE bool operator < (const PxsBroadPhaseOverlap& x) const
	{
		return mVolumes < x.mVolumes;
	}

	PXD_FORCE_INLINE bool operator > (const PxsBroadPhaseOverlap& x) const
	{
		return mVolumes > x.mVolumes;
	}

	PXD_FORCE_INLINE bool operator == (const PxsBroadPhaseOverlap& x) const
	{
		return mVolumes == x.mVolumes;
	}

	PXD_FORCE_INLINE PxU32 getVolume0() const 
	{
		return (mVolumes >> 16) & 0xFFFF;
	}

	PXD_FORCE_INLINE PxU32 getVolume1() const 
	{
		return (mVolumes & 0xFFFF);
	}

	PXD_FORCE_INLINE const PxU32& getKey() const
	{
		return mVolumes;
	}

	PXD_FORCE_INLINE void invalidate()
	{
		mVolumes = 0;
	}

	PXD_FORCE_INLINE bool isValid() const
	{
		return (mVolumes != 0);
	}

	PXD_FORCE_INLINE bool isSorted() const
	{
		return getVolume0() < getVolume1();
	}

	PXD_FORCE_INLINE void sort()
	{
		const PxU32 volume0 = getVolume0();
		const PxU32 volume1 = getVolume1();
		if (volume1 < volume0)
		{
			mVolumes = volume1 << 16 | volume0;
		}
	}

private:

	PxU32 mVolumes;
};

class PxsBroadPhasePairHolder
{
public:
	
	PxsBroadPhasePairHolder(PxU32 numBuckets = 2048):
	  mNumBuckets(numBuckets),
	  mNumElements(0),
	  mBuckets(0),
	  mTimeStamp(0),
	  mElementPool(1024)
	{
		// ensure Pow2 buckets
		PXN_ASSERT((mNumBuckets & (mNumBuckets-1)) == 0);

		//Allocate the bucket-list
		mBuckets = reinterpret_cast<Element**>(PXN_MALLOC(sizeof(Element*)*mNumBuckets));
		PxcMemset(mBuckets, 0, sizeof(Element*)*mNumBuckets);
	}

	~PxsBroadPhasePairHolder()
	{
		//clear();
		PXN_FREE(mBuckets);
	}

	PXD_INLINE void tick()
	{
		mTimeStamp++;
	}
// Enable to keep bucket Linked Lists sorted
#define PXD_BROADPHASEPAIRHOLDER_SORTED 1
//#define PXD_BROADPHASEPAIRHOLDER_SORTED_ON_ADDR 1

	PXD_INLINE bool insert(const PxsBroadPhaseOverlap& overlap)
	{

#if defined(PXD_BROADPHASEPAIRHOLDER_SORTED)

		const PxUInt bucketID = getBucketIndex(overlap);

		Element* e = mBuckets[bucketID];

		// check if we add it at start
		if (e == NULL || e->mOverlap > overlap)
		{
			// add at start
			Element* n = mElementPool.construct();
			n->mOverlap = overlap;
			n->mTimeStamp = mTimeStamp;
			n->mNext = e;
			mBuckets[bucketID] = n;
			mNumElements++;
			return true;
		}

		if(e->mOverlap == overlap)
		{
			e->mTimeStamp = mTimeStamp;
			return false;
		}

		// find correct element
		while(e->mNext && e->mNext->mOverlap < overlap)
			e = e->mNext;

		// refresh is this correct
		if(e->mNext && e->mNext->mOverlap == overlap)
		{
			e->mNext->mTimeStamp = mTimeStamp;
			return false;
		}

		// otherwise we add it to the end
		Element *n = mElementPool.construct();
		n->mOverlap = overlap;
		n->mTimeStamp = mTimeStamp;
		n->mNext = e->mNext;
		e->mNext = n;
		mNumElements++;
		
		return true; //added

#else

		// See if it exists a bucket
		PxUInt bucketID = getBucketIndex(overlap);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			Element* e = mBuckets[bucketID];
			while(e)
			{
				if(e->mOverlap == overlap)
				{
					//exists, refresh time stamp, return false (no need to add)
					e->mTimeStamp = mTimeStamp;
					return false;	
				}
				e = e->mNext;
			}

			// Does not have, so add it
			e = mElementPool.construct();
			e->mOverlap = overlap;
			e->mTimeStamp= mTimeStamp;
			e->mNext = mBuckets[bucketID];
			// note to self; adding it first might not be a good idea in terms of memory friendliness
			mBuckets[bucketID] = e; 
			mNumElements++;
			return true; //added
		}
		else
		{
			// No bucket, add one
			Element* e = mElementPool.construct();
			e->mOverlap = overlap;
			e->mTimeStamp = mTimeStamp;
			mBuckets[bucketID] = e;
			mNumElements++;
			return true; //added
		}

		return false;

#endif
	}

	PXD_INLINE bool remove(const PxsBroadPhaseOverlap& overlap)
	{
#if defined(PXD_BROADPHASEPAIRHOLDER_SORTED)

		if(mNumElements == 0) return false;

		const PxUInt bucketID = getBucketIndex(overlap);

		Element* e = mBuckets[bucketID];

		// nothing in bucket
		if (e == NULL) return false;

		// first node in bucket
		if(e->mOverlap == overlap)
		{
			mBuckets[bucketID] = e->mNext;
			mElementPool.destroy(e);
			mNumElements--;
			return true;
		}

		if (e->mNext == NULL) return false;

		// find node before the one we want to remove
		while (e->mNext && e->mNext->mOverlap < overlap)
			e = e->mNext;

		// remove if it is a match
		if(e->mNext && e->mNext->mOverlap == overlap)
		{
			Element* const r = e->mNext;
			e->mNext = e->mNext->mNext;
			mNumElements--;
			mElementPool.destroy(r);
			return true;
		}

		return false;

#else

		if(mNumElements == 0) return false; //cannot remove from empty

		// See if it exists a bucket
		PxUInt bucketID = getBucketIndex(overlap);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and remove if needed
			Element* el = mBuckets[bucketID];
			if(el->mOverlap == overlap)
			{
				//Bucket, so remove it
				mBuckets[bucketID] = el->mNext;
				mElementPool.destroy(el);
				mNumElements--;
				return true;
			}

			Element* prevEl = el;
			el = el->mNext;
			while(el)
			{
				if(el->mOverlap == overlap)
				{
					//unlink
					prevEl->mNext = el->mNext;
					mElementPool.destroy(el);
					mNumElements--;
					return true;
				}

				prevEl = el; el = el->mNext;
			}
		}
		// not found
		return false;
#endif
	}

	PXD_INLINE void remove(const PxcArray<PxU32>& sortedVolumes)
	{
		if (mNumElements == 0 || sortedVolumes.getSize() == 0) return;

		PxUInt i;
		for (i = 0; i < mNumBuckets; i++)
		{
			Element* n = mBuckets[i];
			if(n)
			{
				while (n && contains(sortedVolumes, n->mOverlap))
				{
					Element* const r = n;
					n = n->mNext;
					mElementPool.destroy(r);
					mNumElements--;
				}
				mBuckets[i] = n;
				while(n && n->mNext) 
				{
					if (contains(sortedVolumes, n->mNext->mOverlap))
					{
						Element* const r = n->mNext;
						n->mNext = n->mNext->mNext;
						mElementPool.destroy(r);
						mNumElements--;
					} else {
						n = n->mNext;
					}
				}
			}
		}
	}

	PXD_INLINE void purge(PxcArray<PxsBroadPhaseOverlap>* keys)
	{
		if (mNumElements == 0) return;

		PxUInt i;
		for (i = 0; i < mNumBuckets; i++)
		{
			Element* n = mBuckets[i];
			if(n)
			{
				while (n && n->mTimeStamp != mTimeStamp)
				{
					keys->insert(n->mOverlap);
					Element* const r = n;
					n = n->mNext;
					mElementPool.destroy(r);
					mNumElements--;
				}
				mBuckets[i] = n;
				while(n && n->mNext) 
				{
					if (n->mNext->mTimeStamp != mTimeStamp)
					{
						keys->insert(n->mNext->mOverlap);
						Element* const r = n->mNext;
						n->mNext = n->mNext->mNext;
						mElementPool.destroy(r);
						mNumElements--;
					} 
					else 
					{
						n = n->mNext;
					}
				}
			}
		}
	}

private:

	PxU32 mTimeStamp;

	struct Element
	{
		Element() : mNext(0)
		{
		}

		PxsBroadPhaseOverlap mOverlap;
		PxU32 mTimeStamp;
		Element* mNext;
	};

	PxU32 mNumBuckets;
	Element** mBuckets;

	PxU32 mNumElements;
	PxcSimplePool<Element> mElementPool;

	PXD_FORCE_INLINE PxUInt getBucketIndex(const PxsBroadPhaseOverlap& overlap) const
	{
		//return PxcHashFunction(overlap.getKey()) & (mNumBuckets-1);
		return (overlap.getVolume0() + overlap.getVolume1()) & (mNumBuckets-1);
	}

	PXD_FORCE_INLINE bool contains(const PxcArray<PxU32>& sortedVolumes, const PxsBroadPhaseOverlap& overlap) const
	{
		const PxU32 volume0 = overlap.getVolume0();
		const PxU32 volume1 = overlap.getVolume1();
		const PxU32* pCurr = sortedVolumes.getPtr();
		const PxU32* const pLast = sortedVolumes.getPtr() + sortedVolumes.getSize();
		while(pCurr < pLast)
		{
			const PxU32 volume = *pCurr++;
			if (volume > volume0 && volume > volume1) return false;
			if (volume == volume0 || volume == volume1) return true;
		}
		return false;
	}
};

#endif
