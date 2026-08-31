#ifndef PXC_DISTANCEPOINTLINE_H
#define PXC_DISTANCEPOINTLINE_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcSegment.h"

PxFloat PxcDistancePointSegmentSquared(const PxcVector& segmentOrigin, 
									   const PxcVector& segmentExtent, 
									   const PxcVector& point, 
									   PxFloat* param=NULL);

PXD_FORCE_INLINE PxFloat PxcDistancePointSegmentSquared(const PxcSegment& segment, 
														const PxcVector& point, 
														PxFloat* param=NULL)
{
	return PxcDistancePointSegmentSquared(segment.point0, segment.direction(), point, param);
}


#endif
