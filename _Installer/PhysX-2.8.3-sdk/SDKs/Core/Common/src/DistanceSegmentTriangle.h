#ifndef NX_DISTANCE_SEGMENT_TRIANGLE
#define NX_DISTANCE_SEGMENT_TRIANGLE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxSegment.h"

//namespace NxCollision {

NxReal segmentTriangleSqrDist(const NxSegment & segment, 
							  const NxVec3 & p0, const NxVec3 & p1, const NxVec3 & p2,
							  NxReal * t = 0, NxReal * u = 0, NxReal * v = 0);

NX_INLINE NxReal segmentTriangleDist(const NxSegment & segment, 
									 const NxVec3 & p0, const NxVec3 & p1, const NxVec3 & p2,
									 NxReal * t = 0, NxReal * u = 0, NxReal * v = 0)
	{
	return NxMath::sqrt(segmentTriangleSqrDist(segment, p0, p1, p2, t, u, v));
	}

//}

#endif
