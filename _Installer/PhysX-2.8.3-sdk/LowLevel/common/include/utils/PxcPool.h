#ifndef PXC_POOL_H
#define PXC_POOL_H

#include <new>
#include "PxnMemory.h"
#include "PxnError.h"
#include "PxcHelperFunction.h"
#include "PxcBitMap.h"
#include "PxcSet.h"


/*!
Simple allocation pool
*/
template<class T>
class PxcSimplePool
{
public:
	PxcSimplePool (PxU32 elementsPerSlab = 32)
		: mElementsPerSlab (elementsPerSlab), mSlabSize (mElementSize * elementsPerSlab),
		mFreeElement (0)
	{
	}

	~PxcSimplePool ()
	{
		disposeElements();
	}

	PXD_FORCE_INLINE T* construct()
	{
		T* t = reinterpret_cast<T*>(allocateElement());
		
		if(t == 0)
			return 0;

		new (t) T;

		return t;
	}

	template<class A1>
	PXD_FORCE_INLINE T* construct(const A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1, class A2>
	PXD_FORCE_INLINE T* construct(const A1& a, const A2& b)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b);

		return t;
	}

	template<class A1, class A2, class A3>
	PXD_FORCE_INLINE T* construct(const A1& a, const A2& b, const A3& c)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b, c);

		return t;
	}


	PXD_FORCE_INLINE void destroy(T* const p)
	{
		if(p == 0)
			return;

		p->~T();
		freeElement(p);
	}

	void clear()
	{
		mFreeElement = 0;

		// bubble sort slabs on address
		bool sorted;
		do
		{
			sorted = true;
			PxU32 i;
			for (i = 1; i<mAllocatedSlabs.getSize(); i++)
			{
				if (mAllocatedSlabs[i-1] > mAllocatedSlabs[i])
				{
					sorted = false;
					PxcSwap(mAllocatedSlabs[i-1], mAllocatedSlabs[i]);
				}
			}
		} while(!sorted);

		PxU32 i;
		for (i = 0; i<mAllocatedSlabs.getSize(); i++)
		{
			PxU8* slab = (PxU8*)mAllocatedSlabs[mAllocatedSlabs.getSize()-i-1];

			// Build a chain of nodes for the freelist
			FreeList* nextFree = mFreeElement;
			PxU8* node = slab + (mElementsPerSlab - 1) * mElementSize;
			for (; node >= slab; node -= mElementSize)
			{
				FreeList* element = reinterpret_cast<FreeList*> (node);
				element->mNext = nextFree;
				nextFree = element;
			}
			mFreeElement = nextFree;
		}

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
	PxcSet<void*> mAllocatedSlabs;

	PxU32 mElementsPerSlab;
	PxU32 mSlabSize;

	FreeList* mFreeElement; // Head of free-list

	
	// Helper function to get bitmap of allocated elements

	// Allocate a slab and segregate it into the freelist
	PXD_INLINE void allocateSlab ()
	{
		PxU8* slab = reinterpret_cast<PxU8*>(PXN_MALLOC(mSlabSize));

		//Save the slab ptr
		mAllocatedSlabs.insert(slab);

		// Build a chain of nodes for the freelist
		FreeList* nextFree = mFreeElement;
		PxU8* node = slab + (mElementsPerSlab - 1) * mElementSize;
		for (; node >= slab; node -= mElementSize)
		{
			FreeList* element = reinterpret_cast<FreeList*> (node);
			element->mNext = nextFree;
			nextFree = element;
		}
		mFreeElement = nextFree;
	}

	// Free a slab
	PXD_INLINE void freeSlab (void* slab)
	{
		mAllocatedSlabs.remove(slab);
		PXN_FREE(slab);
	}

	// Free all slabs
	PXD_INLINE void freeAllSlabs ()
	{
		for(PxUInt i = 0; i < mAllocatedSlabs.getSize(); ++i)
		{
			PXN_FREE(mAllocatedSlabs[i]);
		}
		mAllocatedSlabs.release();

		mFreeElement = 0;
	}

	// Allocate space for single object
	PXD_FORCE_INLINE void* allocateElement ()
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
	PXD_FORCE_INLINE void freeElement (void* p)
	{
		FreeList* element = reinterpret_cast<FreeList*>(p);
		element->mNext = mFreeElement;
		mFreeElement = element;
	}

	/*
	Cleanup method. Go through all active slabs and call destructor for live objects,
	then free their memory
	*/
	PXD_INLINE void disposeElements ()
	{
		// Build a set of the free nodes
		PxcSet<void*> freeNodeSet;
		while(mFreeElement)
		{
			freeNodeSet.insert(mFreeElement);
			mFreeElement = mFreeElement->mNext;
		}

		if(freeNodeSet.getSize() != mElementsPerSlab * mAllocatedSlabs.getSize())
		{
			for(PxUInt i = 0; i < mAllocatedSlabs.getSize(); ++i)
			{
				PxU8* slab = reinterpret_cast<PxU8*>(mAllocatedSlabs[i]);
				for(PxUInt elId = 0; elId < mElementsPerSlab; ++elId)
				{
					void* element = slab + elId*mElementSize;
					if(freeNodeSet.contains(element))
						continue; //already freed, skip it

					T* t = reinterpret_cast<T*>(element);
					t->~T(); // Destroy
				}
			}
		}

		freeAllSlabs();
	}
};

/*!
Allocator for pools of data structures
Also decodes indices (which can be computed from handles) into objects. To make this
faster, the EltsPerSlab must be a power of two
*/
template <class T, class ArgumentType> 
class PxcPoolList
{
public:
	PxcPoolList(ArgumentType* argument, PxU32 eltsPerSlab, PxU32 maxSlabs):
		mEltsPerSlab(eltsPerSlab), 
		mMaxSlabs(maxSlabs), 
		mSlabCount(0),
		mFreeList(0), 
		mFreeCount(0), 
		mArgument(argument)
	{
		PXN_ASSERT(mEltsPerSlab>0);
		// either maxSlabs = 1 (non-resizable pool), or elts per slab must be a power of two
		PXN_ASSERT(maxSlabs==1 || maxSlabs < 8192 && (mEltsPerSlab & (mEltsPerSlab-1)) == 0);
		mSlabs = reinterpret_cast<T**>(PXN_MALLOC(mMaxSlabs * sizeof(T*)));
		mLog2EltsPerSlab = 0;

		if(mMaxSlabs>1)
		{
			for(mLog2EltsPerSlab=0; mEltsPerSlab!=(PxU32)1<<mLog2EltsPerSlab; mLog2EltsPerSlab++)
				;
		}
	}

	~PxcPoolList()
	{
		// Run all destructors
		for(PxU32 i=0;i<mSlabCount;i++)
		{
			T* slab = mSlabs[i];
			for(PxU32 j=0;j<mEltsPerSlab;j++)
			{
				slab[j].~T();
			}
		}

		//Deallocate
		for(PxU32 i=0;i<mSlabCount;i++)
			PXN_FREE(mSlabs[i]);
		PXN_FREE(mFreeList);
		PXN_FREE(mSlabs);
	}

	// TODO: would be nice to add templated construct/destroy methods like PxcObjectPool

	PXD_INLINE T *get()
	{
		if(mFreeCount == 0 && !extend())
			return 0;
		T* element = mFreeList[--mFreeCount];
		PxU32 i = element->getIndex();
		mUseBitmap.set(i);
		return element;
	}

	PXD_INLINE void put(T* element)
	{
		PxU32 i = element->getIndex();
		mUseBitmap.clear(i);
		mFreeList[mFreeCount++] = element;
	}

	PXD_FORCE_INLINE T* findByIndex(PxU32 index) const
	{
		if(index>=mSlabCount*mEltsPerSlab || !(mUseBitmap.check(index)))
			return 0;
		return mMaxSlabs==1 ? mSlabs[0]+index : mSlabs[index>>mLog2EltsPerSlab] + (index&(mEltsPerSlab-1));
	}

	PXD_FORCE_INLINE T* findByIndexFast(PxU32 index) const
	{
		PXN_ASSERT(findByIndex(index) != 0);
		PXN_ASSERT(mMaxSlabs != 1);

		return mSlabs[index>>mLog2EltsPerSlab] + (index&(mEltsPerSlab-1));
	}

	bool extend()
	{
		if(mSlabCount == mMaxSlabs)
			return false;
		T * mAddr = reinterpret_cast<T*>(PXN_MALLOC(mEltsPerSlab * sizeof(T)));
		if(!mAddr)
			return false;
		mSlabs[mSlabCount++] = mAddr;

		if(mFreeList)
			PXN_FREE(mFreeList);
		mFreeList = reinterpret_cast<T**>(PXN_MALLOC(mSlabCount * mEltsPerSlab * sizeof(T*)));

		// Make sure the usage bitmap is up-to-size
		mUseBitmap.clear(mSlabCount*mEltsPerSlab-1); //set last element as not used
	
		// Add to free list in descending order so that lowest indices get allocated first - 
		// the FW context code currently *relies* on this behavior to grab the zero-index volume
		// which can't be allocated to the user. TODO: fix the aforementioned hack.

		for(PxI32 i=mEltsPerSlab-1;i>=0;i--)
			mFreeList[mFreeCount++] = new(mAddr+i) T(mArgument,(mSlabCount-1) * mEltsPerSlab + i);

		return true;
	}

	PXD_INLINE PxU32 getMaxUsedIndex()	const
	{
		/*
		PxU32 maxIdx = mSlabCount*mEltsPerSlab;
		while(!mUseBitmap.check(maxIdx--));
		return maxIdx;*/
		return mUseBitmap.getHighestSetBit();
	}

	PXD_INLINE PxcBitMapIterator getIterator() const
	{
		return PxcBitMapIterator(mUseBitmap);
	}

private:
	PxU32					mEltsPerSlab;
	PxU32					mMaxSlabs;
	PxU32					mSlabCount;
	PxU32					mLog2EltsPerSlab;
	T**						mFreeList;
	PxU32					mFreeCount;
	T**						mSlabs;
	ArgumentType*			mArgument;
	PxcBitMap				mUseBitmap;
};

#endif
