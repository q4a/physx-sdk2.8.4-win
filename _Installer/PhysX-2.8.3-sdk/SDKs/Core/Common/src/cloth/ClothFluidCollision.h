#ifndef CLOTH_FLUID_COLLISION_H
#define CLOTH_FLUID_COLLISION_H

#include "Physics.h"
#include "NxFoundation.h"
#include "Array.h"
#include "ClothHash.h"

#if NX_USE_FLUID_API

class NvCloth;
class NvFluid;
class NpScene;

// ------------------------------------------------------------------------------

class ClothFluidCollision {
public: 
	ClothFluidCollision(NvCloth *cloth) : mCloth(cloth) {}
	~ClothFluidCollision() {};

	bool handleCollisions(void* positions, void* velocities, NxU32 numParticles, NxU32 particleBytes,
		NxReal fromFluidResponseCoefficient, NxReal toFluidResponseCoefficient);

private:
	bool collectFluids();
	void hashFluids();
	bool detectionAndResponse(void* positions, void* velocities, NxU32 numParticles, NxU32 particleBytes,
		NxReal fromFluidResponseCoefficient, NxReal toFluidResponseCoefficient);

	NvCloth *mCloth;

	static const NxU32 mMaxFluids = 10;
	NxU32 mNumFluids;
	NvFluid *mFluids[mMaxFluids];

	NxReal mCollisionDist;
	ClothHash mHash;

	NxFoundation::NxArraySDK<NxVec3> mFluidUpdates[mMaxFluids];
};

#endif //NX_FLUID_API

#endif
