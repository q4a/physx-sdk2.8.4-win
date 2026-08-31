#ifndef NX_FRAMEWORK_PXVOLUME
#define NX_FRAMEWORK_PXVOLUME
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

/*
A PxVolume represents a broad phase volume. It can track overlaps but not real interaction
type overlaps. It knows nothing of actors and activation.
*/

#include "Px.h"
#include "Array.h"
#include "Allocateable.h"
#include "px_broadphase.h"
#include "PxIterator.h"


class PxScene;
class PxVolumeOverlap;

class PxVolume : public NxFoundation::NxAllocateable
{
	friend class PxVolumeOverlap;
public:
	PxVolume(PxVolumeType type) : mLLVolume(0), mType(type) { }
	virtual ~PxVolume() {};

	// Get an iterator to the overlaps connected to the volume
	NX_INLINE PxIterator<PxVolumeOverlap*const> getOverlaps() const;
	NX_INLINE NxU32 getOverlapCount() const;

	// Gets the low level volume
	NX_INLINE PxdVolume getLowLevelVolume() const;
	// Sets the low level volume
	NX_INLINE void setLowLevelVolume(PxdVolume volume);
	// Create the low level volume
	void createLowLevelVolume(PxdVolumeDesc& volumeDesc, PxScene& scene);
	// Destroy the low level volume
	NX_INLINE void destroyLowLevelVolume();

	NX_INLINE PxVolumeType getPxVolumeType() const;

private:
	//These are called from overlap creation/destruction
	void registerOverlap(PxVolumeOverlap* overlap);
	void unregisterOverlap(PxVolumeOverlap* overlap);

	PxdVolume mLLVolume;
	PxVolumeType mType;
	NxFoundation::NxArraySDK<PxVolumeOverlap*> mOverlaps;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE PxIterator<PxVolumeOverlap*const> PxVolume::getOverlaps() const 
{ 
	return PxIterator<PxVolumeOverlap*const>(mOverlaps.begin(), mOverlaps.end()); 
}

NX_INLINE NxU32 PxVolume::getOverlapCount() const
{
	return mOverlaps.size();
}

NX_INLINE PxdVolume PxVolume::getLowLevelVolume() const 
{ 
	return mLLVolume; 
}

NX_INLINE void PxVolume::setLowLevelVolume(PxdVolume volume) 
{ 
	mLLVolume = volume; 
}

NX_INLINE void PxVolume::destroyLowLevelVolume()
{
	if (getLowLevelVolume() != 0)
	{
		PxdVolumeDestroy(getLowLevelVolume());
		setLowLevelVolume(0);
	}
}

NX_INLINE PxVolumeType PxVolume::getPxVolumeType() const
{
	return mType;
}

#endif
