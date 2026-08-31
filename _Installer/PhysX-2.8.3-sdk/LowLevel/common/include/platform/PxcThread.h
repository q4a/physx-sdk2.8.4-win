#ifndef PXC_THREAD_H
#define PXC_THREAD_H

#include "px_config.h"
#include "PxcNoCopy.h"
#include "PxcPlatformInclude.h"
#include "PxnMemory.h"

#if defined(PXD_PLATFORM_WINDOWS)
#include "win/PxcThread_WIN.h"
#if WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP
#include "win/Win32Threads.h"
#endif
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcThread_XBOX.h"
#elif defined(PXD_PLATFORM_LINUX)
#include "linux/PxcThread_LINUX.h"
#elif defined(PXD_PLATFORM_CELL)
#include "ps3/PxcThread_PS3.h"
#else
	#error "Define mutex for your platform"
#endif

/**
Thread abstraction API that supports Win32 and Linux threads
*/

typedef void* (*PxcThreadExecuteFn)(void *);

class PxcThread: private PxcNoCopy, public PxnAllocateable
{
public:

	enum Priority 
	{
		PriorityTimeCritical,
		PriorityHigh,
		PriorityAboveNormal,
		PriorityNormal,
		PriorityBelowNormal,
		PriorityLow
	};


	/**  
	     Construct (but do not start) the thread object. Executes in the context
	     of the spawning thread
	*/

	PxcThread(Priority priority = PriorityNormal,
			  PxU32 stackSize = DEFAULT_STACK_SIZE);

	/**  
	     Construct and start the the thread, passing the given arg to the given fn. (pthread style)
	*/

	PxcThread(PxcThreadExecuteFn fn, 
			  void *arg, 
			  Priority priority = PriorityNormal, 
			  PxU32 stackSize = DEFAULT_STACK_SIZE);


	/**
	    Deallocate all resources associated with the thread. Should be called in the
	    context of the spawning thread.
	*/

	virtual ~PxcThread();


	/**
	    start the thread running. Called in the context of the spawning thread.
	*/

	void start();

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
	virtual PxU32 setAffinityMask(PxU32 mask, PxU32 procIDHint = 0xffffffff);


	void setPriority(Priority prio);

	/** Put the current thread to sleep for the given number of milliseconds */

	static void sleep(PxU32 ms);

	static PxcThreadId getCurrentThreadId();

private:
    class PxcThreadRep *mRep;

	enum State
	{
		NotStarted,
		Started,
		Stopped
	};

	PxcThreadExecuteFn		mFn;
	void *					mArg;
	PxcThread::Priority		mPriority;
	PxU32					mStackSize;	
	PxcThread::State		mState;

};

#endif
