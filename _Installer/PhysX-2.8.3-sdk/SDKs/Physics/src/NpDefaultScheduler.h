#ifndef NX_PHYSICS_NP_DEFAULT_SCHEDULER
#define NX_PHYSICS_NP_DEFAULT_SCHEDULER

#include "NxScene.h"
#include "NxScheduler.h"
#include "NxThread.h"
#include "NxInterlocked.h"
#include "NxSync.h"
#include "NxMutex.h"
#include "NxSpinMutex.h"

#include "Allocateable.h"


/*
This class impliments a scheduler  to provide the pollForWork()/resetScene()/SDK managed threads. 
The core only knows about a pluggable scheduler(either a user defined one or this one).
*/
class NpDefaultScheduler : public NxUserScheduler
	{
	public:

		NpDefaultScheduler();
		~NpDefaultScheduler();

		virtual void addTask(NxTask *task);
		virtual void addBackgroundTask(NxTask *task);

		virtual void waitTasksComplete();

		
		NxThreadPollResult pollForWork(NxThreadWait waitType);
		NxThreadPollResult pollForBackgroundWork(NxThreadWait waitType);

		void resetSimulation();
		void endSimulation();
		void shutdownWorkerThreads();

	private:

		/*
			There are two choices when waiting for all threads to finish tasks:-
				
				* Spin until the threads finish (ENABLE_WAIT_TASKS==true)
				* Spin a bit(WAIT_TASKS_SPIN) then wait on an event.

			Spinning performs better, but may not play as nice with other threads on the same cpu.
			Makes the most difference if groups get large and the thread has to block for a significant time.
		*/
		static const NxU32 WAIT_TASKS_SPIN=4000;
		static const bool ENABLE_WAIT_TASKS=false;

		NX_INLINE void setFinishedTask()
			{
			if(ENABLE_WAIT_TASKS)
				{
					if(NxInterlocked::Decrement(&workingCount)==0)
					completeSync.set();
				}
			else
				{
					NxInterlocked::Decrement(&workingCount);
				}
			}

		void flushBackgroundTasks();

		NxTask *getTask();
		NxTask *getBkTask();

		NxSpinMutex schedulerLock;
		typedef NxSpinMutexLock NxSchedulerLock;
		//NxMutex schedulerLock;
		//typedef NxMutexLock NxSchedulerLock;

/**** Guarded by schedulerLock*/
		bool sceneEnded;
		bool shutdownThreads;

		NxTask **taskRing;
		NxU32 ringSize;
		NxU32 ringUsed;
		NxU32 ringHead,ringTail;

		NxTask **bkTaskRing;
		NxU32 bkRingSize;
		NxU32 bkRingUsed;
		NxU32 bkRingHead,bkRingTail;
/***/

		volatile NxI32 workingCount;
		volatile NxI32 bkWorkingCount;
		NxSync completeSync;

		//signalled when there _might_ be work to do

		NxSync pendingOrEndSync;//signalled when there is work to do or the scene ended, signalled when destroyed.
		NxSync pendingSync;//signalled when there is work to do or destroyed.
		
		NxSync bkPendingSync;//signalled when there is background work pending or we are shutting down
	};



#endif
