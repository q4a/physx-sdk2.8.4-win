#ifndef NX_PHYSICS_FLUIDINDEXMAP
#define NX_PHYSICS_FLUIDINDEXMAP

/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#if NX_USE_FLUID_API

#include "NxFoundationSDK.h"

#include "Allocateable.h"


class FluidIdToIndexMap: public NxFoundation::NxAllocateable
{
public:
	FluidIdToIndexMap(NxU16 maxParticles);
	~FluidIdToIndexMap();

	/**
	Needs to be called for every particle that has been added to the particle array.
	It assumes that particles are added at the end, in the order as this method is called.
	*/
	NX_INLINE void addId(NxU16 id)
	{
		NX_ASSERT(mParticleNum <= mMaxParticles && id <= mMaxParticles && mIdToIndex[id]==0xffff);
		mIdToIndex[id] = mParticleNum++;
	}

	/**
	Needs to be called for every particle that has been removed from the particle array.
	*/
	NX_INLINE void removeId(NxU16 id)
	{
		NX_ASSERT(mParticleNum <= mMaxParticles && id <= mMaxParticles && mIdToIndex[id]!=0xffff);
		mIdToIndex[id] = 0xffff;
		mParticleNum--;
	}

	/**
	updates the id to index map after particles have been reordered and some particles have been removed.
	*/
	void	update(const NxU16* ids, NxU32 idByteStride, NxU16 particleNum);

	NX_INLINE NxU16 getIndex(NxU16 id) const  {	return mIdToIndex[id]; }

private:
	NxU16* mIdToIndex;
	NxU16 mParticleNum;
	NxU16 mMaxParticles;
};


#endif	// NX_USE_FLUID_API

#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

