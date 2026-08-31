#ifndef PXC_GENERIC_CACHE_H
#define PXC_GENERIC_CACHE_H

#include "px_config.h"

#include "PxcMath.h"

class PxcGenericCache;

struct PxcCacheIndex
{
	PXD_FORCE_INLINE PxcCacheIndex()
	{
		invalidate();		
	}

	PXD_FORCE_INLINE PxcCacheIndex(PxcGenericCache* owner, PxU32 ptr)
	{
		mOwner = owner;
		mPtr = ptr;
	}

	PXD_FORCE_INLINE void invalidate()
	{
		mOwner = NULL;
		mPtr = 0xffFFffFF;
	}

	PXD_FORCE_INLINE bool isValid()	const
	{
		return mPtr != 0xffFFffFF;
	}

	PxcGenericCache*	mOwner;
	PxU32				mPtr;
};

/*
Another type of cache. This cache is circular and returns a virtual pointer which is advanced each insertion.
When new data it can wrap around and invalidate the current pointer.

TODO: Should we really be using PxU32 here instead of size_t for 64bit platforms?
*/
class PxcGenericCache
{
	public:
										PxcGenericCache();
										~PxcGenericCache();

						bool			init(PxU32 cacheSize);

						PxcCacheIndex	cacheData(PxU32 nbBytes, const void* data=NULL, void** cache=NULL);
						bool			shrinkCache(const PxcCacheIndex& cookie, PxU32 usedSize);
						void*			getData(const PxcCacheIndex& cookie)	const;

	PXD_FORCE_INLINE	void			cacheBegin()	{ marker = writePtr;						}
	PXD_FORCE_INLINE	PxU32			cacheEnd()		{ return PxcAbs(PxI32(writePtr - marker));	}
	PXD_FORCE_INLINE	PxU32			getSize()		{ return cacheSize; }
	private:
						PxU32			cacheSize;
						PxU32			writePtr;	//!< Virtual running index
						PxU32			ptrOwner;	//!< Owner of write pointer
						PxU8*			pool;		//!< Cyclic-array of cacheSize bytes
						PxU32			marker;		//!< 

	// Internal methods
						void			release();
						void*			allocFromCache(PxU32 nbBytes, PxcCacheIndex& cookie);
	PXD_FORCE_INLINE	PxU32			computeRealIndex(PxU32 virtualIndex)	const	{ return cacheSize ? virtualIndex % cacheSize : 0;	}
};

#endif
