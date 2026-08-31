#ifndef NX_FOUNDATION_NXSYNC_WII_H
#define NX_FOUNDATION_NXSYNC_WII_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include <revolution/os.h>
#include <revolution/os/OSMutex.h>
#include "Nx.h"

class NxSyncManager {};

class NxSync
{
public:

	NxSync(NxSyncManager* mgr = 0);

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

private:
	template<class ElemType, int maxEntries>	friend class MultipleSync;
	bool mEvent;
	OSMutex mEventMutex;
	OSCond mCond;
	bool mWait;

};

#endif
