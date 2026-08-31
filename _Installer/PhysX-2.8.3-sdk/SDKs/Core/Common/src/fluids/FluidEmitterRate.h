#ifndef NX_PHYSICS_FLUIDEMITTER_RATE
#define NX_PHYSICS_FLUIDEMITTER_RATE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "FluidEmitter.h"

#if NX_USE_FLUID_API


class FluidEmitterRate : public FluidEmitter
{
public:
						FluidEmitterRate(const NxFluidEmitterDesc &, Fluid &);
	virtual				~FluidEmitterRate();

	void stepConstantRate(NxReal dt);

private:

	void initDenseSites();
	void shuffleDenseSites();
	void initSparseSiteHash(NxU32 numEmit, NxU32 sparseMax);
	NxU32 pickSparseEmissionSite(NxU32 sparseMax);

	NxReal				mParticlesToEmit;
	NxU32*				mSites;
	NxU32				mSitesMax;
};


#endif	// NX_USE_FLUID_API

#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

