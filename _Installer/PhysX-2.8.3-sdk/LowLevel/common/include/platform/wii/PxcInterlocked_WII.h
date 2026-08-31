#ifndef PXC_INTERLOCKED_WII_H
#define PXC_INTERLOCKED_WII_H

#include "PxcSimpleMutex.h"

//! Class to provide simple interlocked operations (not threadsafe).
class PxcInterlocked : private PxcNoCopy
{
public:

	static PXD_FORCE_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		//return InterlockedCompareExchangePointer((volatile PVOID *)dest,exch,comp);
		PxcSimpleMutex::ScopedLock scopedlock(mMutex);
		volatile void *oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
		
		return (void *)oldDest;
	}													
	static PXD_FORCE_INLINE PxI32 CompareExchange(volatile PxI32 *dest,PxI32 exch,PxI32 comp)
	{
		//return (PxI32)InterlockedCompareExchange((volatile LONG *)dest,exch,comp);
		PxcSimpleMutex::ScopedLock scopedlock(mMutex);
		PxI32 oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
	
		return oldDest;
	}

	static PXD_FORCE_INLINE PxI32 Exchange(volatile PxI32 *val,PxI32 val2)
	{
		//return (PxI32)InterlockedExchange((volatile LONG *)val,(LONG)val2);
		PxcSimpleMutex::ScopedLock scopedlock(mMutex);
		PxI32 rv=*val;
		*val=val2;
		val2=rv;

		return rv;
	}

	static PXD_FORCE_INLINE PxI32 Increment(volatile PxI32 *val)
	{
		//return (PxI32)InterlockedIncrement((volatile LONG *)val);
		PxcSimpleMutex::ScopedLock scopedlock(mMutex);
		return ++(*val);
	}

	static PXD_FORCE_INLINE PxI32 Decrement(volatile PxI32 *val)
	{
		//return (PxI32)InterlockedDecrement((volatile LONG *)val);
		PxcSimpleMutex::ScopedLock scopedlock(mMutex);
		return --(*val);
	}
	struct SListEntry
	{
		volatile SListEntry *Next;
	};

	struct SListHeader
	{
		volatile SListEntry *Next;
		volatile PxI32 lock;		
	};

	static PXD_FORCE_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
		header.lock = -1;// -1 == unlocked !=-1 means unlocked
	}

	static PXD_FORCE_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1){}//lock

		entry->Next = header.Next;
		header.Next = entry;

		Exchange(&header.lock,-1);//unlock
	}
	static PXD_FORCE_INLINE SListEntry *PopSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1){}//lock

		volatile SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;

		Exchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static PXD_FORCE_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1){}//lock

		volatile SListEntry *rv = header.Next;
		header.Next = NULL;

		Exchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static PXD_FORCE_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return (SListEntry *)item->Next;
	}

private:
	PxcInterlocked();
	static PxcSimpleMutex mMutex;
};

#endif
