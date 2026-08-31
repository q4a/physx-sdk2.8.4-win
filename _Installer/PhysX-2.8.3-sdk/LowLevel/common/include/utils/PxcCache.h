#ifndef PXC_CACHE_H
#define PXC_CACHE_H

#include "px_config.h"

#include "PxcHashFunction.h"

/*!
Class for use with a PxcCache to generate a hash for a pointer.
*/
template<class T>
struct PxcPointerHasher
{
	static PXD_FORCE_INLINE PxUInt hash(T ptr)
	{
		return PxcHashFunction(ptr);
	}
	static PXD_FORCE_INLINE bool equal(T a, T b)
	{
		return a==b;
	}

	static PXD_FORCE_INLINE T invalidKey()
	{
		return NULL;
	}
};

/*!
Class which maintains a cache of objects, consisting of a key/value pair. Uses a hash table, collisions
are handled by over writing the old value with a new key/value pair.
*/
template<class KeyType,class ValueType,class HasherType>
class PxcCache
{
public:

	// initial size must be a power of 2.
	PXD_INLINE PxcCache(PxUInt initialSize)
	{
		mSize = initialSize;
		mCacheData = (CacheItem *) PXN_MALLOC(mSize * sizeof(CacheItem));
		PxcMemset(mCacheData,0,sizeof(CacheItem)*mSize);
		mTimeStamp = 1;
	}

	PXD_INLINE ~PxcCache()
	{
		PXN_FREE(mCacheData);
	}

	PXD_FORCE_INLINE bool get(const KeyType& key, ValueType& value)
	{
		PxUInt index = getIndex(key);

		if(  mCacheData[index].timeStamp == mTimeStamp && HasherType::equal(key, mCacheData[index].key))
		{
			value = mCacheData[index].value;
			return true;
		}
		else
			return false;
	}

	PXD_FORCE_INLINE PxUInt getIndex(const KeyType& key)
	{
		PxUInt hashVal = HasherType::hash(key);
		return hashVal & (mSize-1);
	}

	PXD_FORCE_INLINE ValueType* getPtr(PxUInt index)
	{
		return &(mCacheData[index].value);
	}

	PXD_FORCE_INLINE const ValueType* getPtr(PxUInt index) const
	{
		return &(mCacheData[index].value);
	}

	PXD_FORCE_INLINE void get(PxUInt index, ValueType& value) const
	{
		value = mCacheData[index].value;
	}

	PXD_FORCE_INLINE bool isValid(PxUInt index, const KeyType& key)
	{
		return mCacheData[index].timeStamp == mTimeStamp && HasherType::equal(mCacheData[index].key, key);
	}

	PXD_FORCE_INLINE void put(PxUInt index, const KeyType& key, const ValueType& value)
	{
		mCacheData[index].key   = key;
		mCacheData[index].value = value;
		mCacheData[index].timeStamp = mTimeStamp;
	}

	PXD_FORCE_INLINE void put(const KeyType& key, const ValueType& value)
	{
		put(getIndex(key), key, value);
	}

	PXD_FORCE_INLINE void validate(PxUInt index, const KeyType& key)
	{
		mCacheData[index].key = key;
		mCacheData[index].timeStamp = mTimeStamp;
	}

	PXD_FORCE_INLINE void invalidate()
	{
    mTimeStamp++;
    if ( mTimeStamp == 0 )
		{
			for (PxUInt i = 0; i < mSize; i++)
				mCacheData[i].timeStamp = 0;
			mTimeStamp = 1;
		}
	}


private:
	struct CacheItem
		{   
		PxUInt		timeStamp;
		KeyType     key;
		ValueType   value;
		};

	CacheItem*	mCacheData;

	PxUInt	  mSize;
	PxUInt    mTimeStamp;
	};


#endif


