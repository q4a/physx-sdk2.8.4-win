#ifndef PXC_THREADCOHERANTCACHE_H
#define PXC_THREADCOHERANTCACHE_H

#include "PxcNoCopy.h"
#include "PxcInterlocked.h"
#include "PxcSimpleMutex.h"
#include "PxcAlignedMalloc.h"

/*!
Controls a pool of large objects which must be thread safe. 
Tries to return the object most recently used by the thread(for better cache coherancy).
Assumes the object has a default contructor.

(Note the semantics are different to a pool because we dont want to construct/destroy each time
an object is requested, which may be expensive).

TODO: add thread coherancy.
*/
template<class T>
class PxcThreadCoherantCache : public PxcNoCopy
{
public:

	typedef PxcInterlocked::SListEntry EntryBase;

	PXD_INLINE PxcThreadCoherantCache()
	{
		root = (PxcInterlocked::SListHeader*)PxcAlignedMalloc(sizeof(PxcInterlocked::SListHeader), 16);

		PxcInterlocked::InitializeSList(*root);
	}

	PXD_INLINE ~PxcThreadCoherantCache()
	{
		T* np = static_cast<T*>(PxcInterlocked::PopSList(*root));

		while(np!=NULL)
		{
			np->~T();
			PxcAlignedFree(np);
			np = NULL;

			np = static_cast<T*>(PxcInterlocked::PopSList(*root));
		}

		PxcAlignedFree(root);
		root=NULL;
	}

	PXD_INLINE T* get()
	{
		T* rv = static_cast<T*>(PxcInterlocked::PopSList(*root));
		if(rv==NULL)
		{
			rv = (T*)PxcAlignedMalloc(sizeof(T), 16);
			new (rv) T;
		}

		return rv;
	}

	PXD_INLINE void put(T* item)
	{
		PxcInterlocked::PushSList(*root, item);
	}


private:

	PxcInterlocked::SListHeader* root;

	template<class T2>
	friend class PxcThreadCoherantCacheIterator;
};

/*!
Used to iterate over all objects controlled by the cache.

Note: The iterator flushes the cache(extracts all items on construction and adds them back on
destruction so we can iterate the list in a safe manner).
*/
template<class T> 
class PxcThreadCoherantCacheIterator
{
public:
	PxcThreadCoherantCacheIterator(PxcThreadCoherantCache<T>& cache) : mCache(cache)
	{
		mNext = PxcInterlocked::FlushSList(*cache.root);		
		mFirst = mNext;
	}
	~PxcThreadCoherantCacheIterator()
	{

		PxcInterlocked::SListEntry* np = mFirst;
		while(np != NULL)
		{
			PxcInterlocked::SListEntry* npNext = PxcInterlocked::NextSList(np);
			PxcInterlocked::PushSList(*mCache.root, np);
			np = npNext;
		}
	}

	PXD_INLINE T* getNext()
	{
		if(mNext == NULL)
			return NULL;

		T* rv = static_cast<T*>(mNext);
		mNext = PxcInterlocked::NextSList(mNext);

		return rv;
	}
private:

	PxcThreadCoherantCache<T> &mCache;
	PxcInterlocked::SListEntry* mNext;
	PxcInterlocked::SListEntry* mFirst;
	
};

#endif
