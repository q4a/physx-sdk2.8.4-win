#ifndef NX_DISTANCE_SEGMENT_BOX
#define NX_DISTANCE_SEGMENT_BOX
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxCapsule.h"
#include "NxSegment.h"
#include "NxBox.h"


//namespace NxCollision {

	NxReal segmentBoxSqrDist(const NxSegment & segment, const NxVec3 & c0, const NxVec3 & e0, const NxMat33 & r0, 
	  						 NxReal * t = 0, NxVec3 * p = 0);

	NX_INLINE NxReal segmentBoxDist(const NxSegment & segment, const NxVec3 & c0, const NxVec3 & e0, const NxMat33 & r0, 
									NxReal * t = 0, NxVec3 * p = 0)
		{
		return NxMath::sqrt(segmentBoxSqrDist(segment, c0, e0, r0, t, p));
		}

	NX_INLINE NxReal segmentBoxSqrDist(const NxSegment & segment, const NxBox & box, NxReal * t = 0, NxVec3 * p = 0)
		{
		return segmentBoxSqrDist(segment, box.center, box.extents, box.rot, t, p);
		}

	NX_INLINE NxReal segmentBoxDist(const NxSegment & segment, const NxBox & box, NxReal * t = 0, NxVec3 * p = 0)
		{
		return segmentBoxDist(segment, box.center, box.extents, box.rot, t, p);
		}

	NX_INLINE bool intersectBoxCapsule(const NxBox& worldOBB, const NxCapsule& worldCapsule)
		{
		return segmentBoxSqrDist(worldCapsule, worldOBB) < worldCapsule.radius*worldCapsule.radius;
		}
//}

#endif
