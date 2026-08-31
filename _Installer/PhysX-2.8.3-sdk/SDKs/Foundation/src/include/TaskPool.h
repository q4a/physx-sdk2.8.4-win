#ifndef __TASKPOOL__
#define __TASKPOOL__

#include "Nx.h"
#include "NxUserAllocator.h"
#include "NxThread.h"
#include "NxMutex.h"
#include "NxInterlocked.h"
#include "NxAlignedMalloc.h"

#include "Array.h"
#include "Allocateable.h"

class NxTask;

/*
Implimentation of a thread safe task pool. (NxTask derived classes).

T is the actual type of the task(currently NphaseTask or GroupSolveTask).
*/

template<class T> class TaskPool
	{
	const static PxU32 TaskPoolSlabSize=64;

	public:

		typedef NxInterlocked::SListEntry TaskPoolItem;

		NX_INLINE TaskPool()
			{
			//we have to ensure that the list header is 16byte aligned for win64.
			freeTasks = (NxInterlocked::SListHeader *)NxAlignedMalloc(sizeof(NxInterlocked::SListHeader), 16);

			NxInterlocked::InitializeSList(*freeTasks);
			slabArray.reserve(16);
			}

		~TaskPool()
			{
			NxMutexLock lock(slabAllocMutex);

			NxInterlocked::FlushSList(*freeTasks);

			for(NxU32 i=0;i<slabArray.size();i++)
			{
				// call destructors
				for(NxU32 j=0; j<TaskPoolSlabSize; j++)
					slabArray[i][j].~T();

				NxAlignedFree(slabArray[i]);
			}

			slabArray.clear();

			if(freeTasks!=NULL)
				{
					NxAlignedFree(freeTasks);
					freeTasks = NULL;
				}
			}

		T *allocTask()
			{
				T *rv = static_cast<T *>(NxInterlocked::PopSList(*freeTasks));
				if(rv == NULL)
					return static_cast<T *>(allocateSlab());
				else
					return rv;
			}
		void freeTask(T *task)
			{
				NxInterlocked::PushSList(*freeTasks,task);
			}

	private:

		T *allocateSlab()
			{
			//ack, convoluted memory macros.

			//T *newSlab=new T[TaskPoolSlabSize];

				// we must align this memory.
			T *newSlab=(T *)NxAlignedMalloc(sizeof(T)*TaskPoolSlabSize, 16);
			
			new (newSlab) T();

			//we keep one for the caller
			// and build a list of tasks and insert in the free list
			for(NxU32 i=1;i<TaskPoolSlabSize;i++)
			{
				new (&(newSlab[i])) T();
				NxInterlocked::PushSList(*freeTasks, &(newSlab[i]));
			}

			NxMutexLock lock(slabAllocMutex);
			slabArray.pushBack(newSlab);

			return newSlab;
			}

		NxMutex slabAllocMutex;
		NxFoundation::NxArraySDK<T *> slabArray;

		NxInterlocked::SListHeader *freeTasks;
		
	};

#endif
