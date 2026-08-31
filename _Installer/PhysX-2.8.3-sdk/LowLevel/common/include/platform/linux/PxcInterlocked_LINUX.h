#ifndef PXC_INTERLOCKED_LINUX_H
#define PXC_INTERLOCKED_LINUX_H

#if defined(PXD_PLATFORM_LINUX) || defined(PXD_PLATFORM_ANDROID) && defined(PXD_PLATFORM_X86)
#include "PxcSimpleMutex.h"
#include <stdio.h>
#define Pause() asm ("nop")

//! Class to provide simple interlocked operations (not threadsafe).
class PxcInterlocked : private PxcNoCopy
{
public:


    static PXD_FORCE_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
	    __asm__ __volatile__("lock; cmpxchg %3, %0"
				 : "=m"(*dest), "=a"(comp) // output
				 : "a"(comp), "r"(exch), "m"(*dest) // input
				 : "cc", "memory"); // clobber
	    return comp;
	}
													
	static PXD_FORCE_INLINE PxI32 CompareExchange(volatile PxI32 *dest,PxI32 exch,PxI32 comp)
	{
#if __arm__
		PxI32 result, tmp;
		__asm__ ("\n"							      \
			      "0:\tldr\t%1,[%2]\n\t"					      \
			      "cmp\t%1,%4\n\t"						      \
			      "movne\t%0,%1\n\t"					      \
			      "bne\t1f\n\t"						      \
			      "swp\t%0,%3,[%2]\n\t"					      \
			      "cmp\t%1,%0\n\t"						      \
			      "swpne\t%1,%0,[%2]\n\t"					      \
			      "bne\t0b\n\t"						      \
			      "1:"
			      : "=&r" (result), "=&r" (tmp)
			      : "r" (dest), "r" (exch), "r" (comp)
			      : "cc", "memory");

#else
	    __asm__ __volatile__("lock; cmpxchgl %3, %0"
				 : "=m"(*dest), "=a"(comp) // output
				 : "a"(comp), "r"(exch), "m"(*dest) // input
				 : "cc", "memory"); // clobber
#endif
	    return comp;
	}

	static PXD_FORCE_INLINE PxI32 Exchange(volatile PxI32 *val,PxI32 val2)
	{
	    PxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = val2;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
	}

	static PXD_FORCE_INLINE PxI32 Increment(volatile PxI32 *val)
	{
	    PxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = oldVal + 1;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
	}

	static PXD_FORCE_INLINE PxI32 Decrement(volatile PxI32 *val)
	{
	    PxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = oldVal - 1;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
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
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		Exchange(&header.lock,-1);//unlock
	}
	static PXD_FORCE_INLINE SListEntry *PopSList(SListHeader &header)
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
	static PXD_FORCE_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

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


#elif defined(PXD_PLATFORM_OSX) || defined(PXD_PLATFORM_NATIVE_CLIENT) || defined(PXD_PLATFORM_ANDROID) && !defined(PXD_PLATFORM_X86) || defined(PXD_PLATFORM_FLASH) || defined(PXD_PLATFORM_WEBGL)

//! This class does not provide real interlocked functions; it's not thread-safe!
class PxcInterlocked : private PxcNoCopy
{
public:

	static PXD_FORCE_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		volatile void *oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
		return (void *)oldDest;
	}													
	static PXD_FORCE_INLINE PxI32 CompareExchange(volatile PxI32 *dest,PxI32 exch,PxI32 comp)
	{
		PxI32 oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;
		return oldDest;
	}

	static PXD_FORCE_INLINE PxI32 Exchange(volatile PxI32 *val,PxI32 val2)
	{
		PxI32 rv=*val;
		*val=val2;
		val2=rv;

		return rv;
	}

	static PXD_FORCE_INLINE PxI32 Increment(volatile PxI32 *val)
	{
		return ++(*val);
	}

	static PXD_FORCE_INLINE PxI32 Decrement(volatile PxI32 *val)
	{
		return --(*val);
	}

//Implimentation using a spinlock, ideally we would use a 64bit Interlocked op, but win2k lack support:-(
//maybe impliment in asm if this proves a bottleneck.

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
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		Exchange(&header.lock,-1);//unlock
	}
	static PXD_FORCE_INLINE SListEntry *PopSList(SListHeader &header)
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
	static PXD_FORCE_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(CompareExchange(&header.lock, 0, -1)!=-1);//lock

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

	/*static PXD_FORCE_INLINE void MemoryBarrier()
	{
		__lwsync();
	}*/

	PxcInterlocked();
};
#endif

#endif
