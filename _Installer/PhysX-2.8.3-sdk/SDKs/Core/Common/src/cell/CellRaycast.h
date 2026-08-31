/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef _CELLRAYCAST_H_
#define _CELLRAYCAST_H_

#include "Physics.h"
#include "Scene.h"
#include "NxRay.h"
#include "NxUserRaycastReport.h"

#include "Utilities.h"
#include "FoundationSDK.h"

#include "NxDebugRenderable.h"
#include "IceSupport.h"
#include "Utilities.h"
#include "Filtering.h"
#include "Opcode.h"
#include "Shape.h"
#include "NPhaseContextGrab.h"

#include "CellRaycast.shared.h"

#include "CellSPURSSupport.h"

#define CELL_RAYCAST_MAX_USER_SPUS (6)

class CellRaycast : public NxFoundation::NxAllocateable {
private:

	static NxMutex* mSPURSAccessLock;

	Scene* mScene;
	bool mRaycastSPURSTaskStarted;

	unsigned int mMaxQueries;
	CellRaycastInput mCellRaycastInput[CELL_RAYCAST_MAX_USER_SPUS];
	CellRaycastQueryInput* mCellRaycastQueries;
	unsigned int mNumQueries;
	unsigned int mStartedSPUs;

	inline void setShapesPtrAndStartSPU(CellRaycastQueryInput* queries, unsigned int numQueries);

public:
	CellRaycast(){};
	void init(Scene* scene);

	~CellRaycast();

	inline void add(SPU_RAYCAST_QUERY_TYPE type, const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, void* userData);

	void execute(unsigned int numSPUs, NxSceneQueryReport* report, bool allowPPUFallbackIfBusy);
	void executePPUFallback(NxSceneQueryReport* report);
	void finish(NxSceneQueryReport* report);
};

inline void CellRaycast::add(SPU_RAYCAST_QUERY_TYPE queryType, const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, void* userData)
{
	if (mNumQueries+1 > mMaxQueries) {
		mMaxQueries = (mMaxQueries > 0)?mMaxQueries*2:64;
		CellRaycastQueryInput* mCellRaycastQueriesNew = reinterpret_cast<CellRaycastQueryInput*>(NxAlignedMalloc(sizeof(CellRaycastQueryInput)*mMaxQueries, 128));
		memcpy(mCellRaycastQueriesNew, mCellRaycastQueries, mNumQueries*sizeof(CellRaycastQueryInput));
		NxAlignedFree(mCellRaycastQueries);
		mCellRaycastQueries = mCellRaycastQueriesNew;
	}

	mCellRaycastQueries[mNumQueries].mQueryType = queryType;
	mCellRaycastQueries[mNumQueries].mWorldRay = worldRay;
	mCellRaycastQueries[mNumQueries].mGroups = groups;
	mCellRaycastQueries[mNumQueries].mMaxDistance = maxDist;
	mCellRaycastQueries[mNumQueries].mHintFlags = hintFlags;
	mCellRaycastQueries[mNumQueries].mGroupsMask = groupsMask;
	mCellRaycastQueries[mNumQueries].mShapesType = shapesType;
	mCellRaycastQueries[mNumQueries].mUserData = userData;
	mNumQueries++;
}

inline void CellRaycast::setShapesPtrAndStartSPU(CellRaycastQueryInput* queries, unsigned int numQueries)
{
	mCellRaycastInput[mStartedSPUs].mQueries = queries;
	mCellRaycastInput[mStartedSPUs].mNumQueries = numQueries;

	Opcode::StaticPruner* staticPruner = (Opcode::StaticPruner*)mScene->getPruningEngine().GetStaticPruner();
	Opcode::DynamicPruner2* dynamicPruner2 = (Opcode::DynamicPruner2*)mScene->getPruningEngine().GetDynamicPruner();
	
	const Opcode::AABBTree* staticTree = staticPruner->GetAABBTree();
	const Opcode::AABBTree* dynamicTree = dynamicPruner2->GetAABBTree();

	mCellRaycastInput[mStartedSPUs].mStaticPrunerObjects = staticPruner->GetObjects();
	mCellRaycastInput[mStartedSPUs].mStaticPrunerAABBTreeNode = (staticTree!= NULL)?staticTree->GetNodes():NULL;

	mCellRaycastInput[mStartedSPUs].mDynamicPruner2Objects = dynamicPruner2->GetObjects();
	mCellRaycastInput[mStartedSPUs].mDynamicPruner2AABBTreeNode = (dynamicTree!= NULL)?dynamicTree->GetNodes():NULL;
	mCellRaycastInput[mStartedSPUs].mDynamicPruner2ObjectsAdded = dynamicPruner2->GetAddedPrunables();
	mCellRaycastInput[mStartedSPUs].mNbDynamicPruner2ObjectsAdded = dynamicPruner2->GetNbAddedPrunables();
	mCellRaycastInput[mStartedSPUs].mScene = mScene;

	g_SPURSRaycast.sendRequest(1, (unsigned int)&mCellRaycastInput[mStartedSPUs], 0);

	mStartedSPUs++;
}

#endif // _CELLRAYCAST_H_







