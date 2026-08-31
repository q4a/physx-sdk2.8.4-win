#ifndef NX_DISTANCE_POINT_TRIANGLE
#define NX_DISTANCE_POINT_TRIANGLE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"


//namespace NxCollision {

NxReal pointTriangleSqrDist(const NxVec3 & point, 
							const NxVec3 & p0, const NxVec3 & p1, const NxVec3 & p2,
							NxReal * u = NULL, NxReal * v = NULL);

NX_INLINE NxReal pointTriangleDist(const NxVec3 & point, 
								   const NxVec3 & p0, const NxVec3 & p1, const NxVec3 & p2,
								   NxReal * u = NULL, NxReal * v = NULL)
	{
	return NxMath::sqrt(pointTriangleSqrDist(point, p0, p1, p2, u, v));
	}

//}

#endif
