#ifndef PXS_SUBTASK_H
#define PXS_SUBTASK_H

#include "PxcNoCopy.h"
#include "PxcSimpleMutex.h"
#include "PxnMemory.h"
#include "PxcFastMemory.h"
#include "PxcInterlocked.h"
#include "PxcArray.h"

class PxsContactManager;
class PxsContext;
class PxsDynamicsContext;
class PxsBodyAtom;

/*!
A sub task designed to execute a batch of contact manager updates.

NOTE: we should add some throtling to restrict the number of group tasks queued. ie if we reach a limit
execute some of the groups until the number reduces. Otherwise we may consume a lot of memory just for task book
keeping.
*/
class PxsCMUpdateSubTask
{
public:

	PXD_INLINE PxsCMUpdateSubTask(PxsContext* owner, PxFloat dt)
	{
		mCmCount=0;
		mOwner=owner;
		mDt=dt;
		//Not strictly needed, however is a very useful debugging aid so keep until we measure perf.
		PxcMemset(mCmArray,0,sizeof(PxsContactManager *)*BATCH_SIZE);
	}

	PXD_INLINE ~PxsCMUpdateSubTask()
	{
	}

	PXD_FORCE_INLINE void insert(PxsContactManager* cm)
	{
		mCmArray[mCmCount++]=cm;
	}

	PXD_FORCE_INLINE PxUInt getSize()		const
	{
		return mCmCount;
	}

	PXD_FORCE_INLINE PxUInt getMaxSize()	const
	{
		return BATCH_SIZE;
	}


	static void subTaskExecuteFunc(void* subTaskData);

	static const PxUInt BATCH_SIZE=32;

private:
	
	//we inline this into the callback function to avoid an extra call
	PXD_FORCE_INLINE void execute();

	PxsContactManager*	mCmArray[BATCH_SIZE];
	PxUInt				mCmCount;
	PxFloat				mDt;		//we could probably retrieve from context to save space?
	PxsContext*			mOwner;
};

/*!
A sub task to run group solving in parallel.
*/
class PxsIslandSubTask
{
public:
	
	PXD_INLINE PxsIslandSubTask(PxsDynamicsContext* owner, PxsBodyAtom *const*groupArray, PxUInt groupCount)
	{
		mOwner=owner;
		mGroupCount=groupCount;
		mGroupArray=groupArray;
	}

	PXD_FORCE_INLINE PxU32 getSize()	const
	{
		return mGroupCount;
	}

	static void subTaskExecuteFunc(void* subTaskData);
private:
	
	//we inline this into the callback function to avoid an extra call
	PXD_FORCE_INLINE void execute();

	PxsBodyAtom *const*	mGroupArray;
	PxUInt				mGroupCount;

	PxsDynamicsContext*	mOwner;
};

/*!
A thread safe pool of sub tasks.
*/
template<class T>
class PxsSubTaskPool : public PxcNoCopy
{
public:


	PXD_INLINE PxsSubTaskPool()
	{
		PxcInterlocked::InitializeSList(subTaskList);
		PxcInterlocked::InitializeSList(slabList);
	}

	PXD_INLINE ~PxsSubTaskPool()
	{
		PxcInterlocked::SListEntry* nextSlab = PxcInterlocked::FlushSList(slabList);

		while(nextSlab != NULL)
		{
			ElementSlab *slab = static_cast<ElementSlab *>(nextSlab);
			nextSlab = PxcInterlocked::NextSList(nextSlab);

			PXN_FREE(slab);
		}

	}

	template<class T0>
	PXD_INLINE T* construct(T0& p0)
	{
		Element* elem=allocateElem();
		new (elem) Element(p0);
		return elem;
	}

	template<class T0,class T1>
	PXD_INLINE T* construct(T0& p0, T1& p1)
	{
		Element* elem=allocateElem();
		new (elem) Element(p0,p1);
		return elem;
	}

	template<class T0,class T1,class T2>
	PXD_INLINE T* construct(T0& p0, T1& p1, T2& p2)
	{
		Element* elem=allocateElem();
		new (elem) Element(p0,p1,p2);
		return elem;
	}

	PXD_INLINE void destroy(T* ptr)
	{
		Element* freeMe=static_cast<Element*>(ptr);
		freeMe->~Element();

		PxcInterlocked::PushSList(subTaskList, freeMe);		
	}

private:

	static const PxUInt SLAB_SIZE=32;

	//we may want to remove the next member(and overlay with T) in future to save memory.
	struct Element : T, PxcInterlocked::SListEntry
	{
		PXD_FORCE_INLINE Element() : T() {}

		template<class T0> 
		PXD_FORCE_INLINE Element(T0& p0) : T(p0) {}

		template<class T0, class T1>
		PXD_FORCE_INLINE Element(T0& p0, T1& p1) : T(p0, p1) {}

		template<class T0, class T1, class T2>
		PXD_FORCE_INLINE Element(T0& p0, T1& p1, T2& p2) : T(p0, p1, p2) {}
	};

	struct ElementSlab : PxcInterlocked::SListEntry
	{
		Element data[SLAB_SIZE];
	};

	PXD_INLINE Element* allocateElem()
	{
		PxcInterlocked::SListEntry* elem = PxcInterlocked::PopSList(subTaskList);
		if(elem == NULL)
			return allocateSlab();
		else
			return static_cast<Element *>(elem);
	}

	PXD_INLINE Element* allocateSlab()
	{
		ElementSlab* newSlab=(ElementSlab*)PXN_MALLOC(sizeof(ElementSlab));

		for(PxU32 i=1; i<SLAB_SIZE; i++)
			PxcInterlocked::PushSList(subTaskList, newSlab->data + i);

		Element* rv = newSlab->data;

		PxcInterlocked::PushSList(slabList, newSlab);
		return rv;
	}

	PxcInterlocked::SListHeader subTaskList;
	PxcInterlocked::SListHeader slabList;
	
};

#endif
