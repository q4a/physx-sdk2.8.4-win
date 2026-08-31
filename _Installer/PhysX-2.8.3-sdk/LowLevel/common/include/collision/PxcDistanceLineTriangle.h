#ifndef PXC_DISTANCELINETRIANGLE_H
#define PXC_DISTANCELINETRIANGLE_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcSegment.h"

PxFloat PxcDistanceSegmentTriangleSquared(const PxcVector& segmentOrigin, 
										  const PxcVector& segmentExtent,
										  const PxcVector& triangleOrigin, 
										  const PxcVector& triangleEdge0, 
										  const PxcVector& triangleEdge1,
										  PxFloat* t=NULL, 
										  PxFloat* u=NULL, 
										  PxFloat* v=NULL);

PXD_FORCE_INLINE PxFloat PxcDistanceSegmentTriangleSquared(const PxcSegment& segment, 
														   const PxcVector& triangleOrigin, 
														   const PxcVector& triangleEdge0, 
														   const PxcVector& triangleEdge1,
														   PxFloat* t=NULL, 
														   PxFloat* u=NULL, 
														   PxFloat* v=NULL)
{
	return PxcDistanceSegmentTriangleSquared(segment.point0, segment.direction(), triangleOrigin, triangleEdge0, triangleEdge1, t, u, v);
}

#endif
