#ifndef PXC_SEMAPHORE_WIN_H
#define PXC_SEMAPHORE_WIN_H

#include "px_config.h"
#include "PxcNoCopy.h"
#include "PxcArray.h"

/* NxSync was underspecified, so let's not make that mistake here. 
 * An infinite-counting semaphore, should be easy to implement on
 * Win32, Linux, Mac, Xenon.
 */

class PxcSemaphore: private PxcNoCopy
{
public:
	static const PxU32 FOREVER = 0xffffffff;
	static const PxI32 TIMEOUT = -1;
	static const PxU32 MAX_OBJECTS = 64;

	PxcSemaphore(PxU32 initialValue = 0);
	~PxcSemaphore();

	/** Wait on the object for at most the given number of ms. Returns 
	 *  true if the object is signaled.
	 */

	bool wait(PxU32 milliseconds = FOREVER);

	/** Signal the semaphore */

	void signal();

	/** Wait on an array of objects */

	static PxI32 waitMultiple(PxcArray<PxcSemaphore *> &, bool waitAll, PxU32 milliseconds = FOREVER);

private:
    HANDLE mHandle;
};

#endif
