#ifndef NX_PHYSICS_FLUIDPARTICLEQUEUE
#define NX_PHYSICS_FLUIDPARTICLEQUEUE

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


class FluidParticleQueue: public NxFoundation::NxAllocateable
{
public:

	FluidParticleQueue(NxU16 maxParticles);
	~FluidParticleQueue();

	void addParticle(NxU16 id, NxReal deathTime);
	void removeParticle(NxU16 id);
	NxReal removeParticles(NxU32 numParticles);
	void clear();


private:

	struct HeapParticle
	{
		HeapParticle():
			id(0xffff),
			deathTime(0.0f)
		{}

		HeapParticle(NxU16 _id, NxReal _deathTime):
			id(_id),
			deathTime(_deathTime)
		{}

		NxU16	id;
        NxReal	deathTime;
	};

	void heapInit();
	void heapSift(NxU16 i);
	void heapUpdate(NxU16 i);
	void heapRemove(NxU16 i);
	void heapInsert(const HeapParticle& particle);
	void heapTest();


	HeapParticle*	mHeap;
	NxU16			mHeapSize;

	

	NxU16*			mId2HeapIndex;

	NxU16			mMaxParticles;
};


#endif	// NX_USE_FLUID_API

#endif



//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright ?2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND
