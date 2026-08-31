/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef NX_FOUNDATION_NXSYNC_PS3_H
#define NX_FOUNDATION_NXSYNC_PS3_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include <sys/synchronization.h>

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
	bool evTrigger;
	sys_cond_t condition;
	sys_mutex_t mutex;

};

#endif


