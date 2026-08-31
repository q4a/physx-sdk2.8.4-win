#ifndef NX_COLLISION_RAWBOUNDSOVERLAP
#define NX_COLLISION_RAWBOUNDSOVERLAP
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "PxVolumeOverlap.h"

class RawBoundsVolume;
class NvUserRawBoundsReport;

class RawBoundsElementOverlap : public PxVolumeOverlap
{
public:
	NX_INLINE RawBoundsElementOverlap(RawBoundsVolume &volume0, PxElement &volume1) : PxVolumeOverlap(volume0, volume1)	{};
	NX_INLINE ~RawBoundsElementOverlap() {};
	NX_INLINE RawBoundsVolume& getRawBoundsVolume() const { return static_cast<RawBoundsVolume&>(getVolume0()); }
	NX_INLINE PxVolume& getVolume() const { return getVolume1(); }
private:
};

class RawBoundsOverlap : public PxVolumeOverlap
{
public:
	NX_INLINE RawBoundsOverlap(RawBoundsVolume &volume0, RawBoundsVolume &volume1) : PxVolumeOverlap(volume0, volume1) {};
	NX_INLINE ~RawBoundsOverlap() {};
	NX_INLINE RawBoundsVolume& getRawBoundsVolume0() const { return static_cast<RawBoundsVolume&>(getVolume0()); }
	NX_INLINE RawBoundsVolume& getRawBoundsVolume1() const { return static_cast<RawBoundsVolume&>(getVolume1()); }
private:
};

#endif
