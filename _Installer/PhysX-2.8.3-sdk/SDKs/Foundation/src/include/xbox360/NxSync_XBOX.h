#ifndef NX_FOUNDATION_NXSYNC_XBOX_H
#define NX_FOUNDATION_NXSYNC_XBOX_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxXBOX.h"

#include "Nx.h"

class NxSync
{
public:

	NxSync();

	~NxSync();

	/** Wait on the object for at most the given number of ms. Returns 
	*  true if the object is signaled. -1 means wait forever or until the object 
	*  is signaled.
	*/

	bool wait(NxU32 milliseconds = (NxU32) -1);
	static NX_BOOL waitAny(NxSync** syncArray,NxU32 count,NxU32 milliseconds=-1);

	/** Signal the synchronization object, waking all threads waiting on it */

	void set();

	/** Reset the synchronization object */

	void reset();

	static const NxU32 MAX_WAIT_OBJECTS=32;

	/**	Expose the underlying synchronization object to allow e.g. waits on multiple objects. 
	* This is OS-specific. 
	*/

	/* Win32 method exposes handle to a Win32 event */
	HANDLE getHandle();

private:
	HANDLE mHandle;

};

#endif
