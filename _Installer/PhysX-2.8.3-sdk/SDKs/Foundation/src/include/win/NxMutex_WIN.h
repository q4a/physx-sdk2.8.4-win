/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#ifndef NX_PHYSICS_NXMUTEX_WIN_H
#define NX_PHYSICS_NXMUTEX_WIN_H

/**
Synchronization (mutex) API that supports Win32 and Linux
*/

#include "Allocateable.h"

class NxMutex : public NxFoundation::NxAllocateable
{
  public:
    /**
    The constructor for NxMutex creates a mutex. It is initially
    unlocked.
    */
    NxMutex();

    /**
    The destructor for NxMutex deletes the mutex.
    */
    ~NxMutex();

    /**
    Acquire (lock) the mutex. If the mutex is already locked
    by another thread, this method blocks until the mutex is
    unlocked.
    */
    bool lock();

    /**
    Acquire (lock) the mutex. If the mutex is already locked
    by another thread, this method returns false without blocking.
    */
    bool trylock();

    /**
    Release (unlock) the mutex.
    */
    bool unlock();

  private:
	  class NxMutexRep *mRep;
};

/* grab class (for people who like acquire-init-release-destruct) */

class NxMutexLock : public NxFoundation::NxAllocateable
{
	NxMutex &mMutex;

public:
	NxMutexLock(NxMutex &mutex): mMutex(mutex) 
	{
		mMutex.lock(); 
	}
	
	~NxMutexLock() 
	{
		mMutex.unlock();
	}
};

#endif