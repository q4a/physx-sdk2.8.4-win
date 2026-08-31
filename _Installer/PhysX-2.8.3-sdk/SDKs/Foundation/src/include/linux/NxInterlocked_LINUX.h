#ifndef NX_FOUNDATION_NXINTERLOCKED_LINUX_H
#define NX_FOUNDATION_NXINTERLOCKED_LINUX_H

#include "Nx.h"

#if defined(LINUX) && !defined(ANDROID) && !defined(__flash__) && !defined(__webgl__) // not mac nor android, see below.
#include <stdio.h>
#include <pthread.h>
#define Pause() asm ("nop")

class NxInterlocked
{
public:

        // Warning: The pointer cmpxchg need to be updated when using 64bit pointers (no -m32 used)
	static NX_INLINE void *CompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
	    __asm__ __volatile__("lock; cmpxchg %3, %0"
				 : "=m"(*dest), "=a"(comp) // output
				 : "a"(comp), "r"(exch), "m"(*dest) // input
				 : "cc", "memory"); // clobber
	    return comp;
	}
										      

	static NX_INLINE NxI32 CompareExchange(volatile NxI32 *dest,NxI32 exch,NxI32 comp)
	{
#if __arm__
		// TODO: these are terrible... replace this with something real!
		static pthread_mutex_t atomicLock = PTHREAD_MUTEX_INITIALIZER;

		pthread_mutex_lock(&atomicLock);
		NxI32 oldVal = *dest;
		if(oldVal == comp)
			*dest = exch;
		pthread_mutex_unlock(&atomicLock);

		/*
		NxI32 result, tmp;
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
		*/
#else
	    __asm__ __volatile__("lock; cmpxchg %3, %0"
				 : "=m"(*dest), "=a"(comp) // output
     			 : "a"(comp), "r"(exch), "m"(*dest) // input
				 : "cc", "memory"); // clobber
#endif
	    return comp;
	  
	}


	static NX_INLINE NxI32 Increment(volatile NxI32 *val)
	{
	    NxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = oldVal + 1;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
	}

	static NX_INLINE NxI32 Decrement(volatile NxI32 *val)
	{
	    NxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = oldVal - 1;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
	}

	static NX_INLINE NxI32 Add(volatile NxI32 *val,NxI32 delta)
	{
	    NxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = oldVal + delta;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
	}

	static NX_INLINE NxI32 Max(volatile NxI32 *val,NxI32 val2)
	{
	  NxI32 oldVal, newVal;
	  do
	    {
	      Pause();
	      oldVal=*val;
      
	      if(val2>oldVal)
		newVal=val2;
	      else
		newVal=oldVal;
	      
	    }while(CompareExchange(val, newVal, oldVal)!=oldVal);
	  return *val;
	}

	static NX_INLINE NxI32 Exchange(volatile NxI32 *val,NxI32 val2)
	{
	    NxI32 newVal, oldVal;
	    do
	    {
		Pause();
		oldVal = (*val);
		newVal = val2;
	    } while(CompareExchange(val, newVal, oldVal)!=oldVal);
	    return newVal;
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
		header.lock = -1;// -1 == unlocked, !=-1 means locked
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
	NxInterlocked(){}
};


#else

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

#endif
