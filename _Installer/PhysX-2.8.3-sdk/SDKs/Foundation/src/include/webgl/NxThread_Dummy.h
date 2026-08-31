#ifndef NX_FOUNDATION_NXTHREAD_DUMMY_H
#define NX_FOUNDATION_NXTHREAD_DUMMY_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"

#	define DEFAULT_STACK_SIZE 65536
#	define DEFAULT_OS_STACK_SIZE 65536
#       include <pthread.h>
#if defined(NX32)
typedef NxU32 NxThreadId;
typedef NxU32 NxTLSId;
#elif defined(NX64)
typedef NxU64 NxThreadId;
typedef NxU64 NxTLSId;
#endif


/**
Thread abstraction API that supports Win32 and Linux threads
*/

typedef void* (*NxThreadExecuteFn)(void *);



class NxThread
{
public:
	/**  
	     Construct (but do not start) the thread object. Executes in the context
	     of the spawning thread
	*/

	NxThread();

	/**  
	     Construct and start the the thread, passing the given arg to the given fn. (pthread style)
	*/

	NxThread(NxThreadExecuteFn fn, void *arg);


	/**
	    Deallocate all resources associated with the thread. Should be called in the
	    context of the spawning thread.
	*/

	virtual ~NxThread();


	/**
	    start the thread running. Called in the context of the spawning thread.
	*/

	void start(NxU32 stackSize);

	/**
	    Violently kill the current thread. Blunt instrument, not recommended since
		it can leave all kinds of things unreleased (stack, memory, mutexes...) Should
		be called in the context of the spawning thread.
	*/

	void kill();

	/**
	    The virtual execute() method is the user defined function that will
	    run in the new thread. Called in the context of the spawned thread.
	*/

	virtual void execute(void);

	/**
	    stop the thread. Signals the spawned thread that it should stop, so the 
		thread should check regularly
	*/

	void signalQuit();

	/**
	    Wait for a thread to stop. Should be called in the context of the spawning
		thread. Returns false if the thread has not been started.
	*/

	bool waitForQuit();

	/**
	    check whether the thread is signalled to quit. Called in the context of the
		spawned thread.
	*/

	bool quitIsSignalled();

	/**
	    Cleanly shut down this thread. Called in the context of the spawned thread.
	*/

	void quit();

	/**
		Change the affinity mask for this thread.  Zero means use some SDK defined default.
		For platforms that do not support affinity masks (Xbox360) an explicit processor index may alternatively be provided.

		Returns previous mask if successful, or zero on failure
	*/	
	virtual NxU32 setAffinityMask(NxU32 mask, NxU32 procIDHint = 0xffffffff);


	void setPriority(NxThreadPriority prio);

	/** Put the current thread to sleep for the given number of milliseconds */

	static void sleep(NxU32 ms);

private:
    class NxThreadRep *mRep;
};

#define NxSpinLockPause()



NX_INLINE NxTLSId NxTlsAlloc()
{
	NX_COMPILE_TIME_ASSERT(sizeof(pthread_key_t)==sizeof(NxTLSId));
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	return (NxTLSId)key;
}

NX_INLINE NxTLSId NxTlsFree(NxTLSId index)
{
	NX_COMPILE_TIME_ASSERT(sizeof(pthread_key_t)==sizeof(NxTLSId));
	pthread_key_t key = (pthread_key_t)index;
	return pthread_key_delete(key);
}

NX_INLINE void *NxTlsGet(NxTLSId index)
{
	NX_COMPILE_TIME_ASSERT(sizeof(pthread_key_t)==sizeof(NxTLSId));
	pthread_key_t key = (pthread_key_t)index;
	return pthread_getspecific(key);
}

NX_INLINE NxTLSId NxTlsSet(NxTLSId index,void *value)
{
	NX_COMPILE_TIME_ASSERT(sizeof(pthread_key_t)==sizeof(NxTLSId));
	pthread_key_t key = (pthread_key_t)index;
	return pthread_setspecific(key, value);
}

NX_INLINE NxThreadId NxGetThreadId()
{
	return 0;
}

#endif 