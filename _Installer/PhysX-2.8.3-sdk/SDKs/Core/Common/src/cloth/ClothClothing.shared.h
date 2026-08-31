/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTH_CLOTHING_SHARED_H
#define CLOTH_CLOTHING_SHARED_H

#ifdef __CELLOS_LV2__
#	include "CellUtil.h"
#endif

struct ClothingProjectionConstraint {
	NxI32 parent;
	NxI32 child;
	NxF32 restLength;
};

struct ClothingProjectionEdge {
	NxI32 adjParticle;
	NxF32 restLength;
	NxF32 traversalLength;
};

#endif


