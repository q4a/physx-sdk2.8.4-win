#ifndef NX_READ_WRITE_MUTEX
#define NX_READ_WRITE_MUTEX
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "FoundationSDK.h"
#include "NxMutex.h"
#include "stdio.h"

/*SD:	another grab class with trylock (for people who like acquire-init-release-destruct) 
		it uses a flag (now it is e null ptr check) in the destructor to verify if the mutex has to be unlocked.
*/
class NxMutexTryLock
{
public:
	
	NX_INLINE NxMutexTryLock(): mMutex(0)
		{
		}
	
	NX_INLINE ~NxMutexTryLock() 
		{
		if(mMutex)
			{
			mMutex->unlock();
			}
		}

	NX_INLINE	void addMutex(NxMutex* mutex)
		{
		mMutex = mutex;
		}

	NX_INLINE	NX_BOOL lockAcquired()
		{
		return (mMutex != 0);
		}

private:
	NxMutex*	mMutex;
};


// readlocks should not cause deadlocks
// the readlocks are only locked by the SDK during the buffer swapping
// or if the user reads a value
// -> all read locks are set and reset within the same user call
// that's why it should be ok to do a trylock only for debug purpose
class NxMutexTryReadLock
{
public:
	NX_INLINE NxMutexTryReadLock(NxMutex &mutex): mMutex(mutex) 
		{
		#ifdef _DEBUG
			if(!mutex.trylock()) 
				{
				NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: lock is still aquired. Deadlock possible!");
				mutex.lock();
				}
		#else
			mutex.lock();
		#endif 
		}
	
	NX_INLINE ~NxMutexTryReadLock() 
		{
		mMutex.unlock();
		}

private:
	NxMutex &mMutex;
};


#define BEGINREAD  // NxMutexTryReadLock tryReadLock(*mReadLock);


#define BEGINWRITE	NxMutexTryLock tryWriteLock;																			\
					if(mWriteLock->trylock())																				\
						{																									\
						tryWriteLock.addMutex(mWriteLock);																	\
						}																									\
					else																									\
						{																									\
						NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
						return;																								\
						}

#define BEGINWRITE_NULL	NxMutexTryLock tryWriteLock;																		\
						if(mWriteLock->trylock())																			\
							{																								\
							tryWriteLock.addMutex(mWriteLock);																\
							}																								\
						else																								\
							{																								\
							NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
							return NULL;																					\
							}

#define BEGINWRITE_X(x)	NxMutexTryLock tryWriteLock;																		\
						if(mWriteLock->trylock())																			\
							{																								\
							tryWriteLock.addMutex(mWriteLock);																\
							}																								\
						else																								\
							{																								\
							NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
							return x;																					\
							}

class ReadWriteMutex
	{
	public:
	
	ReadWriteMutex();
	virtual	~ReadWriteMutex();
	
	NX_INLINE void		setWriteLock(NxMutex* writeLock);
	NX_INLINE void		setReadLock(NxMutex* readLock);
	
	NX_INLINE NxMutex*	getWriteLock();
	NX_INLINE NxMutex*	getReadLock();

	NX_INLINE void		BeginWrite() const;
	NX_INLINE bool		tryBeginWrite() const;
	NX_INLINE void		EndWrite() const;

	NX_INLINE void		BeginRead() const;
	NX_INLINE bool		tryBeginRead() const;
	NX_INLINE void		EndRead() const;

	protected:

	NxMutex*	mWriteLock;
	NxMutex*	mReadLock;
	};

NX_INLINE void ReadWriteMutex::setWriteLock(NxMutex* writeLock)
{
	mWriteLock = writeLock;
}

NX_INLINE void ReadWriteMutex::setReadLock(NxMutex* readLock)
{
	mReadLock = readLock;
}

NX_INLINE NxMutex* ReadWriteMutex::getWriteLock()
{
	return mWriteLock;
}

NX_INLINE NxMutex* ReadWriteMutex::getReadLock()
{
	return mReadLock;
}

NX_INLINE void ReadWriteMutex::BeginWrite() const
{
#ifdef _DEBUG
	if(!mWriteLock->trylock()) 
		{
		NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: write lock is still aquired. Deadlock possible!");
		mWriteLock->lock();
		}
#else
	mWriteLock->lock();
#endif 
}


NX_INLINE bool ReadWriteMutex::tryBeginWrite() const
{
	return mWriteLock->trylock();
}

NX_INLINE void ReadWriteMutex::EndWrite() const
{
	mWriteLock->unlock();
}

NX_INLINE void ReadWriteMutex::BeginRead() const
{
/*
#ifdef _DEBUG
	if(!mReadLock->trylock()) 
		{
		NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: read lock is still aquired. Deadlock possible!");
		mReadLock->lock();
		}
#else
	mReadLock->lock();
#endif 
*/
}

NX_INLINE bool ReadWriteMutex::tryBeginRead() const
{
return true;
//	return mReadLock->trylock();
}

NX_INLINE void ReadWriteMutex::EndRead() const
{
//	mReadLock->unlock();
}

#endif
