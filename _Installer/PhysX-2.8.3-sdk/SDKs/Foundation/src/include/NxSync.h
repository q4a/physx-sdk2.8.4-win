#ifndef NX_FOUNDATION_NXSYNC_H
#define NX_FOUNDATION_NXSYNC_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifdef USE_NX_MUTEX
#define NOMINMAX
#include "NxMutex.h"
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

private:
	bool mEvent;
	NxMutex mEventMutex;
	NxMutex mWaitMutex;

};

#elif defined(WIN32)
#include "win/NxSync_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__) || defined(__webgl__)
#include "linux/NxSync_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxSync_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/NxSync_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/NxSync_WII.h"
#elif defined(CAFE)
#include "wiiu/NxSync_WIIU.h"
#endif


#endif
