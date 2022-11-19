#include <stdio.h>
#include <deque>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "NxPhysics.h"
#include "PollingThreads.h"


void PollingThreads::CreateThreads(int count,NxScene *newScene)
	{
	KillThreads();

	quitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	assert(quitEvent!=NULL);

	simRunningEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	assert(simRunningEvent!=NULL);

	workingCounter = -1;
	InitializeCriticalSection(&shieldCS);
	workingMutexEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	threadCount=count;
	threadArray=new HANDLE[threadCount];

	for(int i=0;i<threadCount;i++)
		{
		threadArray[i]=CreateThread(NULL,0,threadFuncStatic,this,0,NULL);
		assert(threadArray[i]!=NULL);
		}

	scene=newScene;
	}

void PollingThreads::ResetPollForWork()
	{
	if(threadArray==NULL)
		return;

	// wait for all threads to stop working.
	WaitForSingleObject(workingMutexEvent, INFINITE);

	scene->resetPollForWork();

	// release threads to start working again.
	SetEvent(simRunningEvent);
	SetEvent(workingMutexEvent);
	
	}

void PollingThreads::KillThreads()
	{
	if(scene!=NULL)
		scene->shutdownWorkerThreads();

	if(quitEvent!=NULL)
		SetEvent(quitEvent);

	if(threadArray!=NULL)
		{
		for(int i=0;i<threadCount;i++)
			{
			if(threadArray[i]!=NULL)
				{
				WaitForSingleObject(threadArray[i],INFINITE);
				CloseHandle(threadArray[i]);
				}
			}

		delete[] threadArray;
		threadArray=NULL;
		scene=NULL;
		}

	if(simRunningEvent!=NULL)
		{
		CloseHandle(simRunningEvent);
		simRunningEvent=NULL;
		}

	if(workingMutexEvent!=NULL)
	{
		CloseHandle(workingMutexEvent);
		workingMutexEvent = NULL;

		DeleteCriticalSection(&shieldCS);
	}

	if(quitEvent!=NULL)
		{
		CloseHandle(quitEvent);
		quitEvent=NULL;
		}
	}


DWORD __stdcall PollingThreads::threadFuncStatic(LPVOID userParam)
	{
	return ((PollingThreads *)userParam)->threadFunc();
	}

DWORD PollingThreads::threadFunc()
	{
	while(true)
		{
		HANDLE handles[]={quitEvent,simRunningEvent};
		if(WaitForMultipleObjects(2,handles,FALSE,INFINITE)==WAIT_OBJECT_0)
			break;//quit event

		// The first thread to enter obtains the working mutex(actually an event).
		EnterCriticalSection(&shieldCS);
		if(InterlockedIncrement(&workingCounter) == 0)
			WaitForSingleObject(workingMutexEvent, INFINITE);
		LeaveCriticalSection(&shieldCS);

		NxThreadPollResult pollResult;

		do
			{
			pollResult=scene->pollForWork(NX_WAIT_SIMULATION_END);
			}while((pollResult==NX_THREAD_MOREWORK)||(pollResult==NX_THREAD_NOWORK));

		ResetEvent(simRunningEvent);//block the first time pollForWork() says the sim ended.

		// The last thread to leave releases the working mutex
		EnterCriticalSection(&shieldCS);
		if(InterlockedDecrement(&workingCounter) < 0)
			SetEvent(workingMutexEvent);
		LeaveCriticalSection(&shieldCS);

		
		}
	return 0;
	}
