#ifndef PXC_DISTANCELINEBOX_H
#define PXC_DISTANCELINEBOX_H

// Line and Segment vs Box distance quieries

#include "px_config.h"
#include "PxcVector.h"
#include "PxcSegment.h"
#include "PxcBox.h"

//! Compute the smallest distance from the (infinite) line to the box.
PxFloat PxcDistanceLineBoxSquared(const PxcVector& lineOrigin,
								  const PxcVector& lineDirection,
								  const PxcVector& boxOrigin,
								  const PxcVector& boxExtent,
								  const PxcMatrix33& boxBase,
								  PxFloat* lineParam=NULL,
								  PxcVector* boxParam=NULL);

//! Compute the smallest distance from the (infinite) line to the box.
PXD_FORCE_INLINE PxFloat PxcDistanceLineBoxSquared(const PxcSegment& line, 
												   const PxcBox& box, 
												   PxFloat* lineParam=NULL, 
												   PxcVector* boxParam=NULL)
{
	return PxcDistanceLineBoxSquared(line.point0, line.direction(), box.origin, box.extent, box.base, lineParam, boxParam);
}

//! Compute the smallest distance from the (finite) line segment to the box.
PxFloat PxcDistanceSegmentBoxSquared(const PxcVector& segmentPoint0,
									 const PxcVector& segmentPoint1,
									 const PxcVector& boxOrigin,
									 const PxcVector& boxExtent,
									 const PxcMatrix33& boxBase,
									 PxFloat* segmentParam=NULL,
									 PxcVector* boxParam=NULL);

//! Compute the smallest distance from the (finite) line segment to the box.
PXD_FORCE_INLINE PxFloat PxcDistanceSegmentBoxSquared(const PxcSegment& segment, 
													  const PxcBox& box, 
													  PxFloat* segmentParam=NULL, 
													  PxcVector* boxParam=NULL)
{
	return PxcDistanceSegmentBoxSquared(segment.point0, segment.point1, box.origin, box.extent, box.base, segmentParam, boxParam);
}

#endif
