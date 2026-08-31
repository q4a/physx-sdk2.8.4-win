#ifndef NX_PHYSICS_FLUIDEMITTER_PRESSURE
#define NX_PHYSICS_FLUIDEMITTER_PRESSURE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "FluidEmitter.h"

#if NX_USE_FLUID_API


class FluidEmitterPressure : public FluidEmitter
{

public:
						FluidEmitterPressure(const NxFluidEmitterDesc &, Fluid &);
	virtual				~FluidEmitterPressure();

	void stepConstantPressure(NxReal dt);
	void clearPredecessors();

private:

	class SiteData: public NxAllocateable
	{
	public:
		NxVec3 position;
		NxVec3 velocity;
		bool predecessor;
		NxReal noiseZ;
	};

private:

	bool stepEmissionSite(NxReal dt, SiteData& siteData, const NxVec3 &sitePos, const NxVec3 &siteVel);
	void predictPredecessorPos(SiteData& siteData, NxReal dt);
	void updatePredecessor(SiteData& siteData, const NxVec3& position, const NxVec3& velocity);

private:

	SiteData*			mSites;
	NxReal				mMaxZNoiseOffset;

};


#endif	// NX_USE_FLUID_API

#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

