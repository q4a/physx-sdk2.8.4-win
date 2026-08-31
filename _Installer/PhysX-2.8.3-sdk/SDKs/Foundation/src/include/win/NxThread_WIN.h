#ifndef NX_FOUNDATION_NXTHREAD_WIN_H
#define NX_FOUNDATION_NXTHREAD_WIN_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"

#define NOMINMAX
#define DEFAULT_STACK_SIZE 1048576
#define DEFAULT_OS_STACK_SIZE 1048576
#	include <windows.h>
typedef NxU32 NxThreadId;
typedef NxU32 NxTLSId;

#if WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP
#include "Win32Threads.h"
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


//This is useful to reduce bus contention on tight spin locks.
#if defined(_WIN64) || defined(_M_ARM)
#	include <intrin.h>
#	define NxSpinLockPause() __nop();
#else
#	define NxSpinLockPause() __asm pause
#endif


//Thread local storage
NX_INLINE NxU32 NxTlsAlloc()
{
	#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	DWORD rv=TlsAlloc();
	#else
	DWORD rv=win32::TlsAlloc();
	#endif
	NX_ASSERT(rv!=TLS_OUT_OF_INDEXES);
	return (NxU32)rv;
}

NX_INLINE NxU32 NxTlsFree(NxU32 index)
{
	#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	BOOL rv=TlsFree(index);
	#else
	BOOL rv=win32::TlsFree(index);
	#endif
	NX_ASSERT(rv);
	return rv;
}

NX_INLINE void *NxTlsGet(NxU32 index)
{
	#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	return TlsGetValue(index);
	#else
	return win32::TlsGetValue(index);
	#endif
}

NX_INLINE NxU32 NxTlsSet(NxU32 index,void *value)
{
	#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	return TlsSetValue(index,value);
	#else
	return win32::TlsSetValue(index,value);
	#endif
}




NX_INLINE NxThreadId NxGetThreadId()
{
	return GetCurrentThreadId();
}

#endif 
