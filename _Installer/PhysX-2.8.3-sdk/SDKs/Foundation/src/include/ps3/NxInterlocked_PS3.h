/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef NX_FOUNDATION_NXINTERLOCKED_PS3_H
#define NX_FOUNDATION_NXINTERLOCKED_PS3_H

#include "Nx.h"

#include <cell/atomic.h>

// Class to provide simple interlocked operations (for Playstation 3)

class NxInterlocked
{
public:

	static NX_INLINE void *CompareExchangePointer(volatile void **dest, void *exch, void *comp)
	{
		//volatile void *oldDest=(*dest);
		//if((*dest)==comp)
		//	(*dest)=exch;
		//return (void *)oldDest;
		void *rv = (void*)cellAtomicCompareAndSwap32((unsigned int*)dest, (unsigned int)comp, (unsigned int)exch);
		//NxMemoryBarrier();
		return rv;
	}


	static NX_INLINE NxI32 CompareExchange(volatile NxI32 *dest, NxI32 exch, NxI32 comp)
	{
		//NxI32 oldDest=(*dest);
		//if((*dest)==comp)
		//	(*dest)=exch;
		//return oldDest;
		NxI32 rv = (NxI32)cellAtomicCompareAndSwap32((unsigned int*)dest, comp, exch);
		//NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Increment(volatile NxI32 *val)
	{
		//return ++(*val);
		NxI32 rv = (NxI32)cellAtomicIncr32((unsigned int*)val) + 1;
		//NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Decrement(volatile NxI32 *val)
	{
		//return --(*val);
		NxI32 rv = (NxI32)cellAtomicDecr32((unsigned int*)val) - 1;
		//NxMemoryBarrier();
		return rv;

	}

	static NX_INLINE NxI32 Add(volatile NxI32 *val,NxI32 delta)
	{
		//(*val)+=delta;
		//return *val;
		NxI32 rv = (NxI32)cellAtomicAdd32((unsigned int*)val, delta);
		//NxMemoryBarrier();
		return rv;
	}

	static NX_INLINE NxI32 Max(volatile NxI32 *val,NxI32 val2)
	{
		//TODO: Need thread safe version for other platforms.
		//if(val2>(*val))
		//	(*val)=val2;
		//return *val;

		//Could do this more efficiently in asm...

		int newValue,oldValue;

		do
			{
			oldValue=*val;
		
			if(val2>oldValue)
				newValue=val2;
			else
				newValue=oldValue;

			//}while(InterlockedCompareExchange((volatile int *)val,newValue,oldValue) != oldValue);
			}while(cellAtomicCompareAndSwap32((unsigned int*)val, oldValue, newValue) != oldValue);

		//NxMemoryBarrier();

		return newValue;

	}

	static NX_INLINE NxI32 Exchange(volatile NxI32 *val,NxI32 val2)
	{
		//NxI32 rv=*val;
		//*val=val2;
		//val2=rv;

		//return rv;
		NxI32 rv = (NxI32)cellAtomicStore32((unsigned int*)val, val2);
		//NxMemoryBarrier();
		return rv;
	}


	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

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

	/*static NX_INLINE void NxMemoryBarrier()
	{
	}*/

	NxInterlocked(){}
};

#endif


