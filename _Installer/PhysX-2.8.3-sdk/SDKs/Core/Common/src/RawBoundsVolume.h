#ifndef NX_COLLISION_RAWBOUNDSELEMENT
#define NX_COLLISION_RAWBOUNDSELEMENT
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxVolume.h"
#include "NvScene.h"

class RawBoundsVolume : public PxVolume, public NvRawBounds
{
public:
	NX_INLINE RawBoundsVolume(PxScene& scene, const NxBounds3 &bounds, NxU32 groupId);
	virtual NX_INLINE ~RawBoundsVolume() {}
	void setBounds(const NxBounds3 &bounds);
	//Implementation of NxAllocatable
	void* operator new(size_t s, void* memory) { return memory; }

protected:
private:
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE RawBoundsVolume::RawBoundsVolume(PxScene& scene, const NxBounds3 &bounds, NxU32 groupId)
: PxVolume(PX_VOLUME_TYPE_RAWBOUNDS)
{
	PxdVolumeDesc volumeDesc;
	PxdVolumeInitDesc(&volumeDesc);
	volumeDesc.type = PXD_VOLUME_TYPE_AABB;
	volumeDesc.group = groupId;
	volumeDesc.userdata = this;
	createLowLevelVolume(volumeDesc, scene);
	NxVec3 vec;
	bounds.getCenter(vec);
	setBounds(bounds);
}

#endif
