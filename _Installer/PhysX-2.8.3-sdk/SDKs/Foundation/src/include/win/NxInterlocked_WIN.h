#ifndef NX_FOUNDATION_NXINTERLOCKED_WIN_H
#define NX_FOUNDATION_NXINTERLOCKED_WIN_H

#include "Nx.h"

#define NOMINMAX
#include <windows.h>

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

//Implementation using a spinlock, ideally we would use a 64bit Interlocked op, but win2k lack support:-(
//maybe implement in asm if this proves a bottleneck.

	struct SListEntry
	{
		volatile SListEntry *Next;
	};

	struct SListHeader
	{
		volatile SListEntry *Next;
		volatile NxI32 lock;		
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
		header.lock = -1;// -1 == unlocked !=-1 means unlocked
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		Exchange(&header.lock,-1);//unlock
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;

		Exchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		header.Next = NULL;

		Exchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return (SListEntry *)item->Next;
	}

private:

	static NX_INLINE void NxMemoryBarrier()
	{
	}
	NxInterlocked(){}
};

#endif
