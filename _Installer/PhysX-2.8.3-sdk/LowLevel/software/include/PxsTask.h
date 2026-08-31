#ifndef PXS_TASK_H
#define PXS_TASK_H


#include "px_task.h"

#include "PxcArray.h"
#include "PxcSimpleMutex.h"

#include "PxnTask.h"

#include "PxsFluidConfig.h"


class PxsContext;
class PxsTaskList;


enum PxsTaskType
{
	PXS_TASK_UPDATE_BROAD_PHASE,
	PXS_TASK_UPDATE_CONTACT_MANAGERS,
	PXS_TASK_UPDATE_DYNAMICS,

#ifdef PXS_ENABLE_FLUIDS
	PXS_TASK_UPDATE_FLUID_SHAPES,
	PXS_TASK_UPDATE_FLUID_DYNAMICS,
	PXS_TASK_UPDATE_FLUID_COLLISION,
#endif

	PXS_TASK_COUNT,

	PXS_TASK_UNDEFINED
};


class PxsTask : public PxnTask
{
public:
											PxsTask();
		virtual								~PxsTask();

		// Implements PxnTask
		virtual			PxdHandle			getHandleV()	const;
		virtual			void				releaseV();
		virtual			PxdTaskStatus		waitV();
		virtual			PxdTaskStatus		getStatusV()	const;
		//~Implements PxnTask

						void				init(PxsTaskList* owner, PxdHandle handle, PxdTaskStatus status);

		PXD_FORCE_INLINE	void			setStatus(PxdTaskStatus status) { mStatus = status; }
		PXD_FORCE_INLINE	PxsTaskType		getType() const { return mType; }
		PXD_FORCE_INLINE	void			setType(PxsTaskType type) { mType = type; }


private:
						PxsTaskType			mType;
						PxdHandle			mHandle;
						PxdTaskStatus		mStatus;
						PxsTaskList*		mOwner;
};


class PxsTaskList
{
public:
											PxsTaskList(PxsContext* context);
		virtual								~PxsTaskList();

						PxsTask&			addTask();
						void				releaseTask(PxdHandle handle);
						PxsTask*			getTask(PxdHandle handle);
						//PxdTaskStatus		wait(PxdTask);
						//PxdTaskStatus		getStatus(PxdTask)	const;


private:
						PxcArray<PxsTask>	mTasks;				// Tasks
						PxcArray<PxU16>		mFreeTaskIndices;	// Stack with indices of free task structures.

						PxsContext*			mContext;

						PxcSimpleMutex		mMutex;
};


#endif
