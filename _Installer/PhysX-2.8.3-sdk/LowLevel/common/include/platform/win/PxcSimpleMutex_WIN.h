#ifndef PXC_SIMPLEMUTEX_WIN_H
#define PXC_SIMPLEMUTEX_WIN_H

//! Simple mutex-class for inter-thread locking
class PxcSimpleMutex : private PxcNoCopy
{
public:
	PxcSimpleMutex();

	~PxcSimpleMutex()
	{
		DeleteCriticalSection(&mCS);
	}

	//! Lock for above mutex
	class ScopedLock : private PxcNoCopy
	{
	public:
		explicit ScopedLock(PxcSimpleMutex& m) :
			mMutex(m)
		{
			EnterCriticalSection(&mMutex.mCS);
		}

		~ScopedLock()
		{
			LeaveCriticalSection(&mMutex.mCS);
		}

	private:
		PxcSimpleMutex& mMutex;
	};


	//! scoped release for above mutex
	class ScopedRelease : private PxcNoCopy
	{
	public:
		explicit ScopedRelease(PxcSimpleMutex& m) :
			mMutex(m)
		{
			LeaveCriticalSection(&mMutex.mCS);
		}

		~ScopedRelease()
		{
			EnterCriticalSection(&mMutex.mCS);
		}

	private:
		PxcSimpleMutex& mMutex;
	};


	// The scoped lock is to be preferred, but sometimes it just ain't enough.
	void lock()
	{
		EnterCriticalSection(&mCS);
	}

	void unlock()
	{
		LeaveCriticalSection(&mCS);
	}

private:
	CRITICAL_SECTION mCS;
};

#endif
