/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTHCLOTHING_PS3_H
#define CLOTHCLOTHING_PS3_H

#include "ClothClothing.shared.h"

struct NxVec3;
struct NxClothConstrainCoefficients;

struct ClothClothingPointers
{
	bool mStretchingProjectionInitialized;
	uint32_t numVelocityUpdate; //only used to find out if we need to update
	NxVec3* mVelocityUpdate;
	NxClothConstrainCoefficients* mConstrainCoefficients;
	uint32_t numProjectionConstraints;
	ClothingProjectionConstraint* mProjectionConstraints;
	uint32_t mMaxEdges;
	
	CELL_MM_PTR(NxU32) clothingProjectionFirstEdgeEA;
	CELL_MM_PTR(bool) clothingProjectionBorderVertexEA;
	//CELL_MM_PTR(NxU32) shapeMatchingSubsetEA;
	//CELL_MM_PTR(NxVec3) shapeMatchingDisplacementsEA;
	//CELL_MM_PTR(ClothingProjectionEdge) clothingProjectionEdgesEA;
	ClothingProjectionEdge* clothingProjectionEdgesEA;
};

#endif


