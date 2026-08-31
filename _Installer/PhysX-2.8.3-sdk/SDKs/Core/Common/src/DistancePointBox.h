#ifndef NX_DISTANCE_POINT_BOX
#define NX_DISTANCE_POINT_BOX
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxSegment.h"
#include "NxBox.h"


//namespace NxCollision {

NxReal pointBoxSqrDist(const NxVec3 & point, const NxVec3 & center, const NxVec3 & extents, 
						const NxMat33 & rot, NxVec3 * pob = 0);

NX_INLINE NxReal pointBoxDist(const NxVec3 & point, const NxVec3 & center, const NxVec3 & extents, 
							   const NxMat33 & rot, NxVec3 * pob = 0)
	{
	return NxMath::sqrt(pointBoxSqrDist(point, center, extents, rot, pob));
	}

NX_INLINE NxReal pointBoxSqrDist(const NxVec3 & point, const NxBox & box, NxVec3 * pob = 0)
	{
	return pointBoxSqrDist(point, box.center, box.extents, box.rot, pob);
	}

NX_INLINE NxReal pointBoxDist(const NxVec3 & point, const NxBox & box, NxVec3 * pob = 0)
	{
	return pointBoxDist(point, box.center, box.extents, box.rot, pob);
	}

//}

#endif




