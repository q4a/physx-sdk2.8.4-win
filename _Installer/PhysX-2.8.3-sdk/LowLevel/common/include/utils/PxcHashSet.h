#ifndef PXC_HASHSET_H
#define PXC_HASHSET_H

#include "PxnMemory.h"
#include "PxnError.h"
#include "PxcFastMemory.h"
#include "PxcHashFunction.h"

/*!
A generic hash set.

A hash set is a specialization of a hash map where the element and the
key are the same thing, so it is a unique non-associative container.

This hashmap have the requirement of needing a hash function and a equality
predicate to function.

Warning: Requires number of buckets to be power of 2
*/
template<class T>
class PxcHashSet
{
public:
	PxcHashSet(size_t initialBucketCount)
		: mNumBuckets((PxU32)initialBucketCount), mNumElements(0), mBuckets(0)
	{
		// ensure Pow2 buckets
		PXN_ASSERT((mNumBuckets & (mNumBuckets-1)) == 0);

		//Allocate the bucket-list
		mBuckets = reinterpret_cast<ElementType**>(PXN_MALLOC(sizeof(ElementType*)*mNumBuckets));
		PxcMemset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
	}

	~PxcHashSet()
	{
		PXN_FREE(mBuckets);
	}

	/*!
	Insert element into hash set. 

	Returns true if element is inserted, false if it already exists in the set
	*/
	PXD_INLINE bool insert(const T& entry)
	{
		// See if it exists a bucket
		PxUInt bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* e = mBuckets[bucketID];
			while(e)
			{
				if(e->mEntry == entry)
					return false;	//exists, no need to add

				e = e->mNext;
			}

			// Does not have, so add it
			e = mHashElementPool.construct();
			e->mEntry = entry;
			e->mNext = mBuckets[bucketID];
			mBuckets[bucketID] = e; 
			mNumElements++;
			return true; //added
		}
		else
		{
			// No bucket, add one
			ElementType* e = mHashElementPool.construct();
			e->mEntry = entry;
			mBuckets[bucketID] = e;
			mNumElements++;
			return true; //added
		}

		return false;
	}
	
	/*!
	Search set for a given value
	*/
	PXD_INLINE bool isElement(const T& entry) const
	{
		// See if there is a bucket we need to search
		PxUInt bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			ElementType* e = mBuckets[bucketID];
			
			while(e && e->mEntry != entry) e = e->mNext;

			//If reached end of bucket, then we didn't find it
			if(!e) return false;
			else return true;
		}
		else
		{
			return false;
		}
	}

	/*!
	Remove element from hash set
	*/
	PXD_INLINE bool remove(const T& entry)
	{
		if(mNumElements == 0)
			return false; //cannot remove from empty set

		// See if it exists a bucket
		PxUInt bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* el = mBuckets[bucketID];
			if(el->mEntry == entry)
			{
				//Bucket, so remove it
				mBuckets[bucketID] = el->mNext;
				mHashElementPool.destroy(el);
				mNumElements--;
				return true;
			}

			ElementType* prevEl = el;
			el = el->mNext;
			while(el)
			{
				if(el->mEntry == entry)
				{
					//unlink
					prevEl->mNext = el->mNext;
					mHashElementPool.destroy(el);
					mNumElements--;
					return true;
				}

				prevEl = el; el = el->mNext;
			}
		}

		return false;
	}

	/*!
	Clear out the set
	*/
	PXD_INLINE void clear()
	{
		if(mNumElements == 0)
			return; //already empty

		for(PxUInt i = 0; i < mNumBuckets; i++)
		{
			ElementType* e = mBuckets[i];
			while(e)
			{
				ElementType* prevE = e;
				e = e->mNext;

				mHashElementPool.destroy(prevE);
			}
		}

		PxcMemset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
		mNumElements = 0;
	}

	/*!
	Get number of elements in hash set
	*/
	PXD_FORCE_INLINE PxUInt getSize() const
	{
		return mNumElements;
	}

	/*!
	Try to set the number of buckets to a new size.
	Remember it should be power of 2.

	Returns false if not being able to resize;
	*/
	PXD_FORCE_INLINE bool tryResize(PxUInt newBucketCount)
	{
		if(mNumElements > 0 || newBucketCount == mNumBuckets)
			return false;

		PXN_ASSERT((newBucketCount & (newBucketCount-1)) == 0);

		mNumBuckets = newBucketCount;
		PXN_FREE(mBuckets);

		mBuckets = reinterpret_cast<ElementType**>(PXN_MALLOC(sizeof(ElementType*)*mNumBuckets));
		PxcMemset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);

		return true;
	}
private:
	PxU32			mNumBuckets;
	PxU32			mNumElements;

	template<class T2>
	struct HashElement
	{
		HashElement()
			: mNext(0)
		{
		}

		T2				mEntry;
		HashElement<T2>*	mNext;
	};
	typedef HashElement<T>	ElementType;

	PxcSimplePool<ElementType>	mHashElementPool;

	ElementType**	mBuckets;

	//Helpers
	PXD_FORCE_INLINE PxUInt getBucketIndex(const T& entry) const
	{
		return PxcHashFunction(entry) & (mNumBuckets-1);
	}

	template<class T3>
	friend class PxcHashSetIterator;
};


/*!
Iterate over the content of a hash set
*/
template<class T>
class PxcHashSetIterator
{
public:
	PxcHashSetIterator(const PxcHashSet<T>& hash)
		: mHash(hash), mBucket(0), mCurrentElement(0)
	{
	}


	/*!
	Get one element from the hash.

	Returns true if any element was returned, else false
	*/
	PXD_INLINE bool get(T& element)
	{
		//try to find next element

		if(mCurrentElement)
		{
			mCurrentElement = mCurrentElement->mNext;
			if(!mCurrentElement) mBucket++;	//next one
		}

		//find new bucket
		if(!mCurrentElement)
		{
			while(!mHash.mBuckets[mBucket] && mBucket < mHash.mNumBuckets)
				mBucket++;

			if(mHash.mNumBuckets == mBucket)
				return false;

			mCurrentElement = mHash.mBuckets[mBucket];
		}

		element = mCurrentElement->mEntry;
		return true;
	}


private:
	typedef PxcHashSet<T>			HashType;
	const HashType&					mHash;
	PxUInt							mBucket;
	typename HashType::ElementType*	mCurrentElement;
};

#endif
