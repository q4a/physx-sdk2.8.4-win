#ifndef NX_FOUNDATION_NXTHREAD_WII_H
#define NX_FOUNDATION_NXTHREAD_WII_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include <revolution/os.h>
#include "Nx.h"

#define DEFAULT_STACK_SIZE 16384*4
#define DEFAULT_OS_STACK_SIZE 65536

typedef NxU32 NxThreadId;
typedef NxU32 NxTLSId;
#define NxSpinLockPause() asm{nop}
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

//Thread local storage

// VangelisK:
//  NX_TLS_MAX_SLOTS defines the max number of TLS slots that can be allocated on the PS3.  This effectively 
//  limits the number of scenes that can be created from the same PPU process.  The PhysicsSDK itself uses 1 
//  TLS slot and each scene that gets created from the game thread uses another 3 TLS slots.  Effectively, the number
//  of scenes that can be active at once is equal to (NX_TLS_MAX_SLOTS-1) / 3.   The CellSlabMemoryManager currently
//  has a limit of 64 scenes and therefore NX_TLS_MAX_SLOTS-1 cannot exceed 64 * 3. The current limit value of 128 slots
//  allows us up to 42 scenes which hopefully will be plenty...
#define NX_TLS_MAX_SLOTS 64

// Expecting the number of threads per slot will be low, one for the scene simulate and probably not 
// many for raycasts.  It seems better to use the space for more slots/scenes.
#define NX_TLS_MAX_THREADS (4)

NxU32 NxTlsAlloc();
NxU32 NxTlsFree(NxU32 index);
void *NxTlsGet(NxU32 index);
NxU32 NxTlsSet(NxU32 index,void *value);



NX_INLINE NxThreadId NxGetThreadId()
{
	return (NxThreadId)OSGetCurrentThread();
}

#endif 
