#ifndef SIMPLE_POOL_H
#define SIMPLE_POOL_H

#include "SortedSet.h"
#include "NxQuickSort.h"

/*!
Simple allocation pool. Ported from LowLevel PxcSimplePool.
*/
template<class T>
class SimplePool
{
public:
	class PtrCompare
	{
	public:
		inline bool operator () (const void* a, const void* b)
		{
			return a < b;
		}
	};

	SimplePool (NxU32 elementsPerSlab = 32)
		: mElementsPerSlab (elementsPerSlab), mSlabSize (mElementSize * elementsPerSlab),
		mFreeElement (0)
	{
	}

	~SimplePool ()
	{
		disposeElements();
	}

	NX_INLINE T* construct()
	{
		T* t = reinterpret_cast<T*>(allocateElement());
		
		if(t == 0)
			return 0;

		new (t) T;

		return t;
	}

	template<class A1>
	NX_INLINE T* construct(const A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1>
	NX_INLINE T* construct(A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1, class A2>
		NX_INLINE T* construct(A1& a, A2& b)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b);

		return t;
	}

	template<class A1, class A2, class A3>
	NX_INLINE T* construct(A1& a, A2& b, A3& c)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b, c);

		return t;
	}

	NX_INLINE void destroy(T* const p)
	{
		if(p == 0)
			return;

		p->~T();
		freeElement(p);
	}

protected:
	struct FreeList
	{
		FreeList* mNext;
	};

	//Some static properties
	enum
	{
		mElementSize = sizeof(T)
	};

    // All the allocated slabs, sorted by pointer
	SortedSet<void*> mAllocatedSlabs;

	NxU32 mElementsPerSlab;
	NxU32 mSlabSize;

	FreeList* mFreeElement; // Head of free-list

	
	// Helper function to get bitmap of allocated elements

	// Allocate a slab and segregate it into the freelist
	NX_INLINE void allocateSlab ()
	{
		NxU8* slab = reinterpret_cast<NxU8*>(NX_ALLOC_PERSISTENT(mSlabSize));

		//Save the slab ptr
		mAllocatedSlabs.insert(slab);

		// Build a chain of nodes for the freelist
		FreeList* nextFree = mFreeElement;
		NxU8* node = slab + (mElementsPerSlab - 1) * mElementSize;
		for (; node >= slab; node -= mElementSize)
		{
			FreeList* element = reinterpret_cast<FreeList*> (node);
			element->mNext = nextFree;
			nextFree = element;
		}
		mFreeElement = nextFree;
	}

	// Free a slab
	NX_INLINE void freeSlab (void* slab)
	{
		mAllocatedSlabs.remove(slab);
		NX_FREE(slab);
	}

	// Free all slabs
	NX_INLINE void freeAllSlabs ()
	{
		for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
		{
			NX_FREE(mAllocatedSlabs[i]);
		}
		mAllocatedSlabs.release();

		mFreeElement = 0;
	}

	// Allocate space for single object
	NX_INLINE void* allocateElement ()
	{
		if(mFreeElement == 0)
		{
			allocateSlab();
		}
		void* element = mFreeElement;

		mFreeElement = mFreeElement->mNext;

		return element;
	}

	// Put space for a single element back in the lists
	NX_INLINE void freeElement (void* p)
	{
		FreeList* element = reinterpret_cast<FreeList*>(p);
		element->mNext = mFreeElement;
		mFreeElement = element;
	}

	/*
	Cleanup method. Go through all active slabs and call destructor for live objects,
	then free their memory
	*/
	NX_INLINE void disposeElements ()
	{
		// Build a set of the free nodes
		FreeList* currentFreeElement = mFreeElement;
		NxU32 countFreeList=0;
		while(currentFreeElement)
		{
			countFreeList++;
			currentFreeElement = currentFreeElement->mNext;
		}

		void** sortedList = mFreeElement ? reinterpret_cast<void**>(NX_ALLOC_TEMP(countFreeList * sizeof(void*))) : NULL;

		countFreeList=0;
		while(mFreeElement)
		{
			sortedList[countFreeList] = mFreeElement;
			mFreeElement = mFreeElement->mNext;
			countFreeList++;
		}

		if (sortedList)
			NxQuickSort<void*, PtrCompare>(sortedList, &sortedList[countFreeList-1]);

		if(countFreeList != mElementsPerSlab * mAllocatedSlabs.getSize())
		{
			for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
			{
				NxU8* slab = reinterpret_cast<NxU8*>(mAllocatedSlabs[i]);
				for(NxU32 elId = 0; elId < mElementsPerSlab; ++elId)
				{
					void* element = slab + elId*mElementSize;

					if(sortedList && contains(sortedList, countFreeList, element))
						continue; //already freed, skip it

					T* t = reinterpret_cast<T*>(element);
					t->~T(); // Destroy
				}
			}
		}

		if (sortedList)
			NX_FREE(sortedList);

		freeAllSlabs();
	}

	NX_INLINE bool contains(void** sortedList, NxU32 elementCount, void* element)
	{
		NxI32 l=0;
		NxI32 r=elementCount-1;
		NxI32 i;
		for(;r>=l;) 
		{
			i=(l+r)/2;
			if(sortedList[i]>element) 
			{
				r=i-1;
			} 
			else 
			{
				if(sortedList[i]==element) 
				{
					return true;
				}
				l=i+1;
			}
		}
		return false;
	}
};

#endif
