#ifndef NX_DISTANCE_SEGMENT_SEGMENT
#define NX_DISTANCE_SEGMENT_SEGMENT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxSegment.h"
#include "NxRay.h"


//namespace NxCollision {

NxReal segmentSegmentSqrDist(const NxSegment & seg0, const NxSegment & seg1, NxReal * s = 0, NxReal * t = 0);

NX_INLINE NxReal segmentSegmentSqrDist(const NxVec3 & seg0_p0, const NxVec3 & seg0_p1,
										const NxVec3 & seg1_p0, const NxVec3 & seg1_p1,
										NxReal * s = 0, NxReal * t = 0)
	{
	return segmentSegmentSqrDist(NxSegment(seg0_p0, seg0_p1), NxSegment(seg1_p0, seg1_p1), s, t);
	}

NX_INLINE NxReal segmentSegmentDist(const NxVec3 & seg0_p0, const NxVec3 & seg0_p1,
									 const NxVec3 & seg1_p0, const NxVec3 & seg1_p1,
									 NxReal * s = 0, NxReal * t = 0)
	{
	return NxMath::sqrt(segmentSegmentSqrDist(NxSegment(seg0_p0, seg0_p1), NxSegment(seg1_p0, seg1_p1), s, t));
	}

NX_INLINE NxReal segmentSegmentDist(const NxSegment & seg0, const NxSegment & seg1, NxReal * s = 0, NxReal * t = 0)
	{
	return NxMath::sqrt(segmentSegmentSqrDist(seg0, seg1, s, t));
	}

NX_INLINE NxReal segmentRaySqrDist(const NxSegment & seg, const NxRay & ray, NxReal * s = 0, NxReal * t = 0)
	{
	return segmentSegmentSqrDist(seg, NxSegment(ray.orig, ray.orig + ray.dir), s, t);
	}

void edgeEdgeDist(NxVec3& x, NxVec3& y,		// closest points
          const NxVec3& p, const NxVec3& a,	// seg 1 origin, vector
          const NxVec3& q, const NxVec3& b);	// seg 2 origin, vector


//}

#endif
