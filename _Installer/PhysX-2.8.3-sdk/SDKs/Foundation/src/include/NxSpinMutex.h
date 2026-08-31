#ifndef __NX_SPIN_MUTEX__
#define __NX_SPIN_MUTEX__

#include "NxThread.h"
#include "NxInterlocked.h"

/*
recursive spin lock.
*/
class NxSpinMutex
	{
	public:

		NX_INLINE NxSpinMutex()
			{
			lockCounter=-1;
			ownerThreadId=0;
			recursionCount=0;
			}

		NX_INLINE ~NxSpinMutex()
			{
			NX_ASSERT(lockCounter==-1);
			NX_ASSERT(ownerThreadId==0);
			NX_ASSERT(recursionCount==0);
			}

		NX_INLINE bool trylock()
			{
			return trylock(NxGetThreadId());
			}

		NX_INLINE void lock()
			{

			//avoid repeated calls the NxGetThreadId().
			NxU32 currThreadId=NxGetThreadId();

			while(!trylock(currThreadId))
				NxSpinLockPause();
			}


		NX_INLINE void unlock()
			{
			NX_ASSERT(ownerThreadId==NxGetThreadId());
			NX_ASSERT(recursionCount>0);

			recursionCount--;
			if(recursionCount==0)
				{
				ownerThreadId=0;

				NxInterlocked::Exchange(&lockCounter,-1);
				}
			}

	private:

		NX_INLINE bool trylock(NxU32 currThreadId)
			{
				if(NxInterlocked::CompareExchange(&lockCounter,0,-1)==-1)
				{
				ownerThreadId=currThreadId;
				recursionCount=1;
				return true;
				}
			else if(ownerThreadId==currThreadId)
				{
				recursionCount++;
				return true;
				}
			else
				return false;
			}


		NxI32 lockCounter;
		NxU32 ownerThreadId;
		NxI32 recursionCount;
	};

class NxSpinMutexLock
	{
	public:

		NX_INLINE NxSpinMutexLock(NxSpinMutex &sm) : spinMutex(sm)
			{
			spinMutex.lock();
			}
		NX_INLINE ~NxSpinMutexLock()
			{
			spinMutex.unlock();
			}	

	private:
		NxSpinMutex &spinMutex;
	};

#endif
