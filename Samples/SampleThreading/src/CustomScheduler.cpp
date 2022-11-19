#include <stdio.h>
#include <deque>

#define NOMINMAX
#if defined(WIN32)
#	include <windows.h>
#else defined(__PPCGEKKO__)
#	include "NxWII.h"
#endif

#include "NxPhysics.h"

#include "CustomScheduler.h"

CustomScheduler::CustomScheduler()
	{
	threadCount=0;
	threadHandles=NULL;
	quitEvent=NULL;
	pendingEvent=NULL;
		
	}
CustomScheduler::~CustomScheduler()
	{
	KillThreads();
	}

void CustomScheduler::CreateThreads(int count)
	{
	KillThreads();

	threadCount=count;
	threadHandles=new HANDLE[threadCount];

	quitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	assert(quitEvent!=NULL);

	pendingEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	assert(pendingEvent!=NULL);

	workingCount=0;

	InitializeCriticalSection(&taskQueueCS);

	for(int i=0;i<threadCount;i++)
		{
		threadHandles[i]=CreateThread(NULL,0,threadFuncStatic,this,0,NULL);

		assert(threadHandles[i]!=NULL);
		}
	}

void CustomScheduler::KillThreads()
	{
	if(threadHandles!=NULL)
		{
		SetEvent(quitEvent);

		DWORD waitResult=WaitForMultipleObjects(threadCount,threadHandles,TRUE,INFINITE);
		assert(waitResult!=WAIT_FAILED);

		CloseHandle(quitEvent);
		quitEvent=NULL;

		CloseHandle(pendingEvent);
		pendingEvent=NULL;

		DeleteCriticalSection(&taskQueueCS);

		delete[] threadHandles;
		threadHandles=NULL;
		}
	}

void CustomScheduler::addTask(NxTask *task)
	{
	EnterCriticalSection(&taskQueueCS);

	taskQueue.push_back(task);
	SetEvent(pendingEvent);
	
	LeaveCriticalSection(&taskQueueCS);
	}
void CustomScheduler::addBackgroundTask(NxTask *task)
	{
	/*
	Background tasks should really go into another task queue so they dont stall processing 
	of time critical tasks.
	*/
	addTask(task);
	}

void CustomScheduler::waitTasksComplete()
	{
	while(executeTask());

	//ensure all threads have finished executing.
	//we spin here, which may not be optimal.
	while(InterlockedCompareExchange(&workingCount,0,0)>0);
	}

DWORD __stdcall CustomScheduler::threadFuncStatic(LPVOID userParam)
	{
	return ((CustomScheduler *)userParam)->threadFunc();
	}

DWORD CustomScheduler::threadFunc()
	{
	HANDLE handles[]={quitEvent,pendingEvent};

	while(true)
		{
		NxTask *task=NULL;

		if(WaitForMultipleObjects(2,handles,FALSE,INFINITE)==WAIT_OBJECT_0)
			break;//quit event
		else
			executeTask();
		}
	return 0;
	}

bool CustomScheduler::executeTask()
	{
	NxTask *task=NULL;

	//task pending
	EnterCriticalSection(&taskQueueCS);

	if(taskQueue.empty())
		ResetEvent(pendingEvent);
	else
		{
		InterlockedIncrement(&workingCount);

		task=taskQueue.front();
		taskQueue.pop_front();
		}

	LeaveCriticalSection(&taskQueueCS);

	if(task!=NULL)
		{
		task->execute();

		InterlockedDecrement(&workingCount);

		return true;
		}
	else
		return false;
	}


