/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef _CELLRAYCAST_SHARED_H_
#define _CELLRAYCAST_SHARED_H_

#include "CellUtil.h"

enum SPU_RAYCAST_QUERY_TYPE {
	SPU_RAYCAST_QUERY_TYPE_ANY,
	SPU_RAYCAST_QUERY_TYPE_CLOSEST
};


struct CellRaycastQueryInput {
	NxRay mWorldRay; //24
	NxU32 mGroups; //28
	float mMaxDistance; //32
	unsigned int mHintFlags; //36
	const NxGroupsMask* mGroupsMask; //40
	unsigned int mShapesType; //44

	unsigned char padTo64[12]; // 56

	// Shared attributes
	SPU_RAYCAST_QUERY_TYPE mQueryType; //60 match CellRaycastQueryOutput.mQueryType
	void* mUserData; //64 match CellRaycastQueryOutput.mUserData
};

struct CellRaycastQueryOutput {
	// Output
	NxRaycastHit mHit; //56

	//unsigned char padTo64[4]; //60

	// Shared attributes
	SPU_RAYCAST_QUERY_TYPE mQueryType; //60
	void* mUserData; //64 match CellRaycastQueryInput.mUserData
};

struct CellRaycastInput {
	CellRaycastQueryInput* mQueries;
	unsigned int mNumQueries;

#ifdef __SPU__
	AABBTreeNode* mStaticPrunerAABBTreeNode;
	Prunable** mStaticPrunerObjects;
	AABBTreeNode* mDynamicPruner2AABBTreeNode;
	Prunable** mDynamicPruner2Objects;
	Prunable** mDynamicPruner2ObjectsAdded;
#else
	const Opcode::AABBTreeNode* mStaticPrunerAABBTreeNode;
	Opcode::Prunable** mStaticPrunerObjects;
	const Opcode::AABBTreeNode* mDynamicPruner2AABBTreeNode;
	Opcode::Prunable** mDynamicPruner2Objects;
	Opcode::Prunable** mDynamicPruner2ObjectsAdded;
#endif
	unsigned int mNbDynamicPruner2ObjectsAdded;

	Scene* mScene;

	unsigned char padTo16[4]; //32
};

#endif // _CELLRAYCAST_SHARED_H_


