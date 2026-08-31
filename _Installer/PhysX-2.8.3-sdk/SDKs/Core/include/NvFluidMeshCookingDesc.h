#ifndef NV_FLUID_MESH_PRE_COOKING_DESC
#define NV_FLUID_MESH_PRE_COOKING_DESC
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxBounds3.h"

class NvFluidMeshCookingDesc
{
public:
	NxBounds3 bounds;
	NxU32 packetSizeMultiplier;
	NxReal restParticlesPerMeter;
	NxReal kernelRadiusMultiplier;
	NxReal motionLimitMultiplier;
	NxReal collisionDistanceMultiplier;
};

#endif
