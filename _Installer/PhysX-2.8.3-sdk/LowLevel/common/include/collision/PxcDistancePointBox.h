#ifndef PXC_DISTANCEPOINTBOX_H
#define PXC_DISTANCEPOINTBOX_H

// Point vs Box distance quieries

#include "px_config.h"
#include "PxcVector.h"
#include "PxcBox.h"

/**
Return the square of the minimum distance from the surface of the box to the given point.
\param point The point
\param boxOrigin The origin of the box
\param boxExtent The extent of the box
\param boxBase The orientation of the box
\param boxParam Set to coordinates of the closest point on the box in its local space
*/
PxFloat PxcDistancePointBoxSquared(const PxcVector& point,
								   const PxcVector& boxOrigin, 
								   const PxcVector& boxExtent, 
								   const PxcMatrix33& boxBase, 
								   PxcVector* boxParam=NULL);

/**
Return the square of the minimum distance from the surface of the box to the given point.
\param point The point
\param box The box
\param boxParam Set to coordinates of the closest point on the box in its local space
*/
PXD_FORCE_INLINE PxFloat PxcDistancePointBoxSquared(const PxcVector& point, 
													const PxcBox& box, 
													PxcVector* boxParam=NULL)
{
	return PxcDistancePointBoxSquared(point, box.origin, box.extent, box.base, boxParam);
}

#endif
