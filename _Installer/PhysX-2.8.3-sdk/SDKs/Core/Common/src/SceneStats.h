#ifndef __SCENE_STATS__
#define __SCENE_STATS__

#include "NxInterlocked.h"
#include "NxSceneStats.h"

/*
Description: Containts statistics for the scene.

Threading: The increment/add functions should be thread safe, the rest/readout do not have to be since 
they are only called from the main thread.
*/
class SceneStats
	{
	NxU32	numPairs;
	NxU32	maxPairs;

	NxU32   numContacts;
	NxU32   maxContacts;
	NxU32   numDynamicActorsInAwakeGroups;
	NxU32   maxDynamicActorsInAwakeGroups;
	NxU32	numAxisConstraints;
	NxU32	maxAxisConstraints;
	NxU32	numSolverBodies;
	NxU32	maxSolverBodies;

	NxU32   numStaticShapes;
	NxU32	maxStaticShapes;
	NxU32	numDynamicShapes;
	NxU32	maxDynamicShapes;

	public:

	SceneStats();

	NX_INLINE void incPairs()				
		{
		NxI32 newValue=NxInterlocked::Increment((volatile NxI32 *)&numPairs);
		NxInterlocked::Max((volatile NxI32 *)&maxPairs,newValue);
		}
	NX_INLINE void decPairs()
		{
		NxInterlocked::Decrement((volatile NxI32 *)&numPairs);
		}

	NX_INLINE void incStaticShapes()
		{
		NxI32 newValue=NxInterlocked::Increment((volatile NxI32 *)&numStaticShapes);
		NxInterlocked::Max((volatile NxI32 *)&maxStaticShapes,newValue);
		}
	NX_INLINE void decStaticShapes()		
		{
		NxInterlocked::Decrement((volatile NxI32 *)&numStaticShapes);
		}

	NX_INLINE void incDynamicShapes()
		{
		NxI32 newValue=NxInterlocked::Increment((volatile NxI32 *)&numDynamicShapes);
		NxInterlocked::Max((volatile NxI32 *)&maxDynamicShapes,newValue);
		}
	NX_INLINE void decDynamicShapes()
		{
		NxInterlocked::Decrement((volatile NxI32 *)&numDynamicShapes);
		}

	void startStep();		//zero counters that count up (nonpersistent objects: contacts, awake objects, axcons, solverBodies.
	void endStep();			//update new maximum for counters that count up.

	NX_INLINE void incContacts()					
		{
		NxInterlocked::Increment((volatile NxI32 *)&numContacts);
		}

	NX_INLINE void addAwakeGroupBodies(NxU32 awbodies)
		{
		NxInterlocked::Add((volatile NxI32 *)&numDynamicActorsInAwakeGroups,awbodies);
		}

	NX_INLINE void addAxCons(NxU32 axcons)
		{
		NxInterlocked::Add((volatile NxI32 *)&numAxisConstraints,axcons);
		}
		
	NX_INLINE void addSolverBodies(NxU32 sbodies)
		{
		NxInterlocked::Add((volatile NxI32 *)&numSolverBodies,sbodies);
		}

	void readOut(NxSceneStats & dest) const;
	};

#endif
