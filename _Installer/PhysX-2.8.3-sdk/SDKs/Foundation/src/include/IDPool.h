#ifndef NX_ID_POOL
#define NX_ID_POOL
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"
#include "Array.h"


class IDPool
	{
			NxU32				currentID;
			NxFoundation::NxArraySDK<NxU32>	freeIDs;
	public:
								IDPool() : currentID(0)	{}

			void				freeID(NxU32 id)
								{
								// Allocate on first call
								// Add released ID to the array of free IDs
								freeIDs.pushBack(id);
								}

			NxU32				getNewID()
								{
								// If recycled IDs are available, use them
								NxU32 size = freeIDs.size();
								if(size)
									{
									if(0)	// O(1) but breaks determinism, unless you delete objects in reverse creation order
										{
										NxU32 id = freeIDs[size-1]; // Recycle last ID
										freeIDs.popBack();
										return id;
										}
									else	// O(n) but doesn't break determinism
										{
										NxU32* ids = freeIDs.begin();
										NxU32 minID=*ids;
										NxU32 pos=0;
										for(NxU32 i=1;i<size;i++)
											{
											if(ids[i]<minID)
												{
												minID=ids[i];
												pos=i;
												}
											}

										if(pos!=size-1)
											{
											NxU32 last = freeIDs.back();
											ids[pos] = last;
											}
										freeIDs.popBack();
										return minID;
										}
									}
								// Else create a new ID
								return currentID++;
								}

			NxI32				getHighestAllocatedID() const
								{
								// highest id is currentID-1 if it is not in the freeIDs list.
								// Could be done faster if we sort the array. Or keep track of the allocated IDs.
								NxI32 ret = NxI32(currentID)-1;
								bool found = true;
								while(found && ret >= 0)
									{
									found = false;
									for(NxU32 i=0;!found&&i<freeIDs.size();i++)
										{
										if(freeIDs[i]==ret)
											found = true;
										}
									if(found)
										ret--;
									}
								return ret;
								}
	};

#endif
