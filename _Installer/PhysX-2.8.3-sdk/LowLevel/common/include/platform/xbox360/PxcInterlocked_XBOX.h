#ifndef PXC_INTERLOCKED_XBOX_H
#define PXC_INTERLOCKED_XBOX_H


//! Class to provide simple interlocked operations.
class PxcInterlocked : private PxcNoCopy
{
public:

	static PXD_FORCE_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		void *rv = InterlockedCompareExchangePointer((volatile PVOID *)dest,exch,comp);
		NxMemoryBarrier();
		return rv;
	}
													
	static PXD_FORCE_INLINE PxI32 CompareExchange(volatile PxI32 *dest,PxI32 exch,PxI32 comp)
	{
		PxI32 rv = (PxI32)InterlockedCompareExchange((volatile LONG *)dest,exch,comp);
		NxMemoryBarrier();
		return rv;
	}

	static PXD_FORCE_INLINE PxI32 Exchange(volatile PxI32 *val,PxI32 val2)
	{
		PxI32 rv = (PxI32)InterlockedExchange((volatile LONG *)val,(LONG)val2);
		NxMemoryBarrier();
		return rv;
	}

	static PXD_FORCE_INLINE PxI32 Increment(volatile PxI32 *val)
	{
		PxI32 rv = (PxI32)InterlockedIncrement((volatile LONG *)val);
		NxMemoryBarrier();
		return rv;
	}

	static PXD_FORCE_INLINE PxI32 Decrement(volatile PxI32 *val)
	{
		PxI32 rv = (PxI32)InterlockedDecrement((volatile LONG *)val);
		NxMemoryBarrier();
		return rv;
	}

	// interlocked list support
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.
	typedef SLIST_ENTRY SListEntry;
	typedef SLIST_HEADER SListHeader;

	static PXD_FORCE_INLINE void InitializeSList(SListHeader &header)
	{
		InitializeSListHead(&header);
	}

	static PXD_FORCE_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		InterlockedPushEntrySList(&header, entry);
	}
	static PXD_FORCE_INLINE SListEntry *PopSList(SListHeader &header)
	{
		return InterlockedPopEntrySList(&header);
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static PXD_FORCE_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		return InterlockedFlushSList(&header);
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static PXD_FORCE_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}

private:
	PxcInterlocked();

	static PXD_FORCE_INLINE void NxMemoryBarrier()
	{
		__lwsync();
	}
};

#endif
