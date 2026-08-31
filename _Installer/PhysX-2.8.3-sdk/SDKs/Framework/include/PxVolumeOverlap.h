#ifndef NX_FRAMEWORK_PXVOLUMEOVERLAP
#define NX_FRAMEWORK_PXVOLUMEOVERLAP
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Allocateable.h"

class PxVolume;

#define PX_INVALID_VOLUME_OVERLAP_ID 0xffff

class PxVolumeOverlap : public NxFoundation::NxAllocateable
{
	friend class PxVolume;
public:
	NX_INLINE PxVolumeOverlap(PxVolume& volume0, PxVolume& volume1);
	NX_INLINE ~PxVolumeOverlap();

	virtual void initialize();
	virtual void destroy();

	NX_INLINE PxVolume& getVolume0() const { return mVolume0; }
	NX_INLINE PxVolume& getVolume1() const { return mVolume1; }

	//Implementation of NxAllocatable
	void* operator new(size_t s, void* memory) { return memory; }
protected:
	NX_INLINE void setVolumeId(PxVolume* volume, NxU16 id);
	NX_INLINE NxU16 getVolumeId(PxVolume* Volume) const;

private:
	PxVolume& mVolume0;
	PxVolume& mVolume1;
	NxU16 mVolumeId0;
	NxU16 mVolumeId1;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE PxVolumeOverlap::PxVolumeOverlap(PxVolume& volume0, PxVolume& volume1) :
mVolume0(volume0), 
mVolume1(volume1), 
mVolumeId0(PX_INVALID_VOLUME_OVERLAP_ID), 
mVolumeId1(PX_INVALID_VOLUME_OVERLAP_ID)
{
}

NX_INLINE PxVolumeOverlap::~PxVolumeOverlap() 
{
}

NX_INLINE void PxVolumeOverlap::setVolumeId(PxVolume* volume, NxU16 id) 
{ 
	NX_ASSERT(id != PX_INVALID_VOLUME_OVERLAP_ID); 
	if (&mVolume0 == volume) 
		mVolumeId0 = id; 
	else 
		mVolumeId1 = id; 
}

NX_INLINE NxU16 PxVolumeOverlap::getVolumeId(PxVolume* volume) const 
{ 
	if (&mVolume0 == volume) 
		return mVolumeId0; 
	else 
		return mVolumeId1; 
}


#endif
