#ifndef NX_LL_THREADING_THUNK
#define NX_LL_THREADING_THUNK

#include "Physics.h"

#include "px_task.h"
#include "px_context.h"

#include "Allocateable.h"
#include "TaskPool.h"

class NxUserScheduler;
class LowLevelThreadingThunk;
class Scene;

class LowLevelThreadingTask : 
	public TaskPool<LowLevelThreadingTask>::TaskPoolItem, public NxTask
{
public:

	NX_INLINE LowLevelThreadingTask()
	{
		owner=NULL;
		executeFunc=NULL;
		subTaskData=NULL;
	}

	NX_INLINE void init(PxdSubTaskExecuteFunc execFunc,void *data,LowLevelThreadingThunk *own)
	{
		owner=own;
		subTaskData=data;
		executeFunc=execFunc;
	}

	virtual void execute();

private:
	LowLevelThreadingThunk *owner;
	PxdSubTaskExecuteFunc executeFunc;

	void *subTaskData;
};

class LowLevelThreadingThunk : public NxFoundation::NxAllocateable
{
public:

	NX_INLINE LowLevelThreadingThunk(PxdContext llContext,NxUserScheduler *scheduler,Scene *scene)
	{
		userScheduler=scheduler;
		mScene = scene;
		PxdContextSetSubTaskCallback(llContext,subtaskCallbackStatic,this);
	}

	NX_INLINE ~LowLevelThreadingThunk()
	{
	}

	NX_INLINE void releaseTask(LowLevelThreadingTask *task)
	{
		taskPool.freeTask(task);
	}

	NX_INLINE Scene *getScene()
	{
		return mScene;
	}
#if XBOX_MULT_THREAD_DEFORMABLE
	NX_INLINE void callback(PxdSubTaskType taskType,PxdSubTaskExecuteFunc executeFunc,void *subTaskData)
	{
		subtaskCallbackStatic(taskType,executeFunc,subTaskData,this);
	}
#endif
private:
	NxUserScheduler *userScheduler;
	Scene *mScene;

	TaskPool<LowLevelThreadingTask> taskPool;

	NX_INLINE LowLevelThreadingTask *createTask(PxdSubTaskExecuteFunc executeFunc,void *subTaskData)
	{
		LowLevelThreadingTask *newTask=taskPool.allocTask();
		newTask->init(executeFunc,subTaskData,this);
		return newTask;
	}

	static void  subtaskCallbackStatic(PxdSubTaskType taskType,PxdSubTaskExecuteFunc executeFunc,void *subTaskData,void *userParam);
};

#endif
