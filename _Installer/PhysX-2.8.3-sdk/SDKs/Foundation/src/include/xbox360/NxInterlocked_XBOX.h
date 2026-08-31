#ifndef NX_FOUNDATION_NXINTERLOCKED_XBOX_H
#define NX_FOUNDATION_NXINTERLOCKED_XBOX_H

#include "Nx.h"

#define NOMINMAX
#include "NxXBOX.h"

class NxInterlocked
{
public:

	static NX_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		void *rv = InterlockedCompareExchangePointer((volatile PVOID *)dest,exch,comp);
		NxMemoryBarrier();
		return rv;
	}
													

	static NX_INLINE NxI32 CompareExchange(volatile NxI32 *dest,NxI32 exch,NxI32 comp)
	{
		NxI32 rv = (NxI32)InterlockedCompareExchange((volatile LONG *)dest,exch,comp);
		NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Increment(volatile NxI32 *val)
	{
		NxI32 rv = (NxI32)InterlockedIncrement((volatile LONG *)val);
		NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Decrement(volatile NxI32 *val)
	{
		NxI32 rv = (NxI32)InterlockedDecrement((volatile LONG *)val);
		NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Add(volatile NxI32 *val,NxI32 delta)
	{
		//Could do this more efficiently in asm...

		LONG newValue,oldValue;

		do
			{
			oldValue=*val;
			newValue=oldValue+delta;
			}while(InterlockedCompareExchange((volatile LONG *)val,newValue,oldValue)!=oldValue);

		NxMemoryBarrier();
		return newValue;
	}

	static NX_INLINE NxI32 Max(volatile NxI32 *val,NxI32 val2)
	{
		//Could do this more efficiently in asm...

		LONG newValue,oldValue;

		do
			{
			oldValue=*val;
		
			if(val2>oldValue)
				newValue=val2;
			else
				newValue=oldValue;

			}while(InterlockedCompareExchange((volatile LONG *)val,newValue,oldValue)!=oldValue);

		NxMemoryBarrier();
		return newValue;
	}

	static NX_INLINE NxI32 Exchange(volatile NxI32 *val,NxI32 val2)
	{
		NxI32 rv = (NxI32)InterlockedExchange((volatile LONG *)val,(LONG)val2);
		NxMemoryBarrier();
		return rv;
	}

	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.
	typedef SLIST_ENTRY SListEntry;
	typedef SLIST_HEADER SListHeader;

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		InitializeSListHead(&header);
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		InterlockedPushEntrySList(&header, entry);
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		return InterlockedPopEntrySList(&header);
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		return InterlockedFlushSList(&header);
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}

private:

	static NX_INLINE void NxMemoryBarrier()
	{
		__lwsync();
	}
	NxInterlocked(){}
};

#endif
