#ifndef PXC_SIMPLEMUTEX_LINUX_H
#define PXC_SIMPLEMUTEX_LINUX_H

#include <pthread.h>
class PxcSimpleMutex : private PxcNoCopy
{
public:
	PxcSimpleMutex()
	{
		//InitializeCriticalSection(&mCS);
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mMutex, NULL);
		pthread_mutexattr_destroy(&attr);
	}

	~PxcSimpleMutex()
	{
		//DeleteCriticalSection(&mCS);
		pthread_mutex_destroy(&mMutex);
	}

	//! Lock for above mutex
	class ScopedLock : private PxcNoCopy
	{
	public:
		explicit ScopedLock(PxcSimpleMutex& m) :
		mMutex(m)
		{
			//EnterCriticalSection(&mMutex.mCS);
			pthread_mutex_lock(&mMutex.mMutex);
		}

		~ScopedLock()
		{
			//LeaveCriticalSection(&mMutex.mCS);
			pthread_mutex_unlock(&mMutex.mMutex);
		}

	private:
		PxcSimpleMutex& mMutex;
	};

	// The scoped lock is to be preferred, but sometimes it just ain't enough.
	void lock()
	{
		//EnterCriticalSection(&mCS);
		pthread_mutex_lock(&mMutex);
	}

	void unlock()
	{
		//LeaveCriticalSection(&mCS);
		pthread_mutex_unlock(&mMutex);
	}

private:
	//CRITICAL_SECTION mCS;
	pthread_mutex_t mMutex;
};



#endif
