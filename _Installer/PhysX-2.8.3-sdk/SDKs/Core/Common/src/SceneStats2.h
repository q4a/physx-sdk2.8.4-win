#ifndef __SCENE_STATS2__
#define __SCENE_STATS2__

#include "NxInterlocked.h"
#include "NxSceneStats2.h"
#include "NvSceneStats2.h"
#include "Array.h"

/*
Description: Contains statistics for the scene.

Threading: The increment/add functions should be thread safe, the rest/readout do not have to be since 
they are only called from the main thread.
*/
class SceneStats2
	{
	private:
		NxI32 bufferedTotalRaycasts;
		NxI32 bufferedTotalSweeps;
		NxI32  bufferedTotalOverlaps;

	public:
		NxFoundation::NxArraySDK<NxSceneStatistic> stats;
		NxFoundation::NxArraySDK<NxU32> mEventIDs;
		NxFoundation::NxArraySDK<NxU32> mSubsetStatIDs;

	SceneStats2();

	NX_INLINE void incStat(SceneStatisticEnum id)				
	{
		NxI32 newValue=++stats[id].curValue;
		stats[id].maxValue = NxMath::max(newValue, stats[id].maxValue);
	}

	NX_INLINE void decStat(SceneStatisticEnum id)
	{
		stats[id].curValue--;
	}

	NX_INLINE void addToStat(SceneStatisticEnum id, NxI32 number)
	{
		NxI32 newValue=stats[id].curValue+=number;
		stats[id].maxValue = NxMath::max(newValue, stats[id].maxValue);
	}

	NX_INLINE void setStat(SceneStatisticEnum id, NxI32 number)
	{
		stats[id].curValue = number;
		stats[id].maxValue = NxMath::max(number, stats[id].maxValue);
	}

	NX_INLINE void incStatInterlocked(SceneStatisticEnum id)				
	{
		NxI32 newValue=NxInterlocked::Increment((volatile NxI32 *)&(stats[id].curValue));
		NxInterlocked::Max((volatile NxI32 *)&(stats[id].maxValue),newValue);
	}

	NX_INLINE void decStatInterlocked(SceneStatisticEnum id)
	{
		NxInterlocked::Decrement((volatile NxI32 *)&(stats[id].curValue));
	}

	NX_INLINE void addToStatInterlocked(SceneStatisticEnum id, NxU32 number)
	{
		NxI32 newValue = NxInterlocked::Add((volatile NxI32 *)&(stats[id].curValue),number);
		NxInterlocked::Max((volatile NxI32 *)&(stats[id].maxValue),newValue);
	}

	NX_INLINE void setStatInterlocked(SceneStatisticEnum id, NxU32 number)
	{
		stats[id].curValue = number;
		NxInterlocked::Max((volatile NxI32 *)&(stats[id].maxValue),number);
	}

	void startStep();		//zero counters that count up (nonpersistent objects: contacts, awake objects, axcons, solverBodies.
	void endStep();			//update new maximum for counters that count up.

	void readOut(NxSceneStats2 & dest) const;

	void createStat(SceneStatisticEnum id, const char *name, const NxU32 parent, bool bSubsetStat);

	void swapBuffers();

    //NX_INLINE NxU32* getEventIDs() { return &mEventIDs[0]; }    //Allows users to get the event IDS for AgPerfmon logging
	
	NX_INLINE void getEventIDs(NxU32** eventIDs, NxU32** subsetStatIDs, NxU32* numSubsetStats) 
	{
		if (eventIDs && subsetStatIDs && numSubsetStats)
		{
			*eventIDs = &mEventIDs[0];
			*subsetStatIDs = &mSubsetStatIDs[0];
			*numSubsetStats = mSubsetStatIDs.size();
		}
	}
};

#endif
