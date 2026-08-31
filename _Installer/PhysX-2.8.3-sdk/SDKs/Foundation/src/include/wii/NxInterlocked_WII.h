#ifndef NX_FOUNDATION_NXINTERLOCKED_WII_H
#define NX_FOUNDATION_NXINTERLOCKED_WII_H

#include "Nx.h"

//Note: this is a non thread safe version, used for platforms without threading support(PS3)
class NxInterlocked
{
public:

	static NX_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		volatile void *oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
		
		return (void *)oldDest;
	}
													

	static NX_INLINE NxI32 CompareExchange(volatile NxI32 *dest,NxI32 exch,NxI32 comp)
	{
		NxI32 oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
	
		return oldDest;
	}

	static NX_INLINE NxI32 Increment(volatile NxI32 *val)
	{
		return ++(*val);
	}

	static NX_INLINE NxI32 Decrement(volatile NxI32 *val)
	{
		//TODO: Need thread safe version for other platforms.
		return --(*val);
	}

	static NX_INLINE NxI32 Add(volatile NxI32 *val,NxI32 delta)
	{
		//TODO: Need thread safe version for other platforms.
		(*val)+=delta;
		return *val;
	}

	static NX_INLINE NxI32 Max(volatile NxI32 *val,NxI32 val2)
	{
		//TODO: Need thread safe version for other platforms.
		if(val2>(*val))
			(*val)=val2;

		return *val;
	}

	static NX_INLINE NxI32 Exchange(volatile NxI32 *val,NxI32 val2)
	{
		NxI32 rv=*val;
		*val=val2;
		val2=rv;

		return rv;
	}


	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

	struct SListEntry
	{
		SListEntry *Next;
	};

	struct SListHeader
	{
		SListEntry *Next;
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		entry->Next = header.Next;
		header.Next = entry;
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;
		return rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		SListEntry *rv = header.Next;
		header.Next = NULL;
		return rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}
private:
	NxInterlocked(){}
};

#endif
