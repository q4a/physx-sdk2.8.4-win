#ifndef NX_SUBTASK_CLOTH
#define NX_SUBTASK_CLOTH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"
#include "Allocateable.h"
#if XBOX_MULT_THREAD_DEFORMABLE
class ClothManager;

class ClothSubTask
{
public:
	NX_INLINE ClothSubTask()
	{
		mOwner=NULL;
		mDt = 0;
		mOffset = 0;		
	}

	NX_INLINE void init(ClothManager*	owner,NxReal dt,NxU32 offset)
	{
		mOwner=owner;
		mDt = dt;
		mOffset = offset;			
	}

	static void subTaskExecuteFunc(void* subTaskData);
private:
	
	//we inline this into the callback function to avoid an extra call
	NX_INLINE void execute();
	ClothManager*	mOwner;
	NxU32			mOffset;
	NxReal			mDt;	
};

NX_INLINE void ClothSubTask::execute()
{
	mOwner->simulate1(mDt,mOffset);	
}

void ClothSubTask::subTaskExecuteFunc(void* subTaskData)
{
	((ClothSubTask*)subTaskData)->execute();
}
#endif

#endif
