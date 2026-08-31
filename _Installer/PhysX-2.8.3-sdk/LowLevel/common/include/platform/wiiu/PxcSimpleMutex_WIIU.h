#ifndef PXC_SIMPLEMUTEX_WIIU_H
#define PXC_SIMPLEMUTEX_WIIU_H

class PxcSimpleMutex : private PxcNoCopy
{
public:
	PxcSimpleMutex()
	{
		//InitializeCriticalSection(&mCS);
		OSInitMutex(&mutex);
	}

	~PxcSimpleMutex()
	{
		//DeleteCriticalSection(&mCS);
	}

	//! Lock for above mutex
	class ScopedLock : private PxcNoCopy
	{
	public:
		explicit ScopedLock(PxcSimpleMutex& m) :
			mMutex(m)
		{
			//EnterCriticalSection(&mMutex.mCS);
			mMutex.lock();
		}

		~ScopedLock()
		{
			//LeaveCriticalSection(&mMutex.mCS);
			mMutex.unlock();
		}

	private:
		PxcSimpleMutex& mMutex;
	};
	
	// The scoped lock is to be preferred, but sometimes it just ain't enough.
	void lock()
	{
		//EnterCriticalSection(&mCS);
		OSLockMutex(&mutex);
	}

	void unlock()
	{
		//LeaveCriticalSection(&mCS);
		OSUnlockMutex(&mutex);
	}

private:
	//CRITICAL_SECTION mCS;
	OSMutex mutex;
};

#endif
