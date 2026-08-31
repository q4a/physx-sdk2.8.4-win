/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTH_HIERARCHY_SHARED_H
#define CLOTH_HIERARCHY_SHARED_H

struct ClothHierarchyLevelConstraint
{
	void set(int p0, int p1) {
		this->p0 = p0; this->p1 = p1;
		summedDistance = 0.0f;
		directDistance = 0.0f;
		deleted = false;
	}
	int p0, p1;
	float summedDistance;	
	float directDistance;
	bool deleted;
};

// ------------------------------------------------------------------------------
struct ClothHierarchyPerParticleInfo
{
	NxU32 level;
	NxU32 numParents;
	NxI32 firstParent;
	NxI32 firstConstraintLink;
};

// ------------------------------------------------------------------
struct ClothHierarchyParticleConstraintLink {
	NxI32 level;
	NxI32 constNr;
	NxI32 next;
	char pad[4];
};


// actually, this is cell specific
#define NX_CLOTH_MAX_HIERARCHY_LEVELS 10 // from NxClothMeshDesc.h

#endif


