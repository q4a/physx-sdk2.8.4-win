/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTHHIERARCHY_PS3_H
#define CLOTHHIERARCHY_PS3_H

#include "CellUtil.h"
#include "ClothHierarchy.shared.h"

struct ClothHierarchyPointers
{
	// hierarchy
	CELL_MM_PTR(ClothHierarchyPerParticleInfo) perParticleInfosEA;
	NxU32 numParticleInfos;

	NxU32 numHierarchyLevels;

	//CELL_MM_PTR(ClothHierarchyLevelConstraint) mLevelConstraints[NX_CLOTH_MAX_HIERARCHY_LEVELS];
	ClothHierarchyLevelConstraint* mLevelConstraints[NX_CLOTH_MAX_HIERARCHY_LEVELS];
	NxI32 mNumLevelConstraints[NX_CLOTH_MAX_HIERARCHY_LEVELS];

	CELL_MM_PTR(NxI32) mParents;
	CELL_MM_PTR(NxReal) mParentWeights;
	
	CELL_MM_PTR(ClothHierarchyParticleConstraintLink) constraintLinksEA;
};

#endif


