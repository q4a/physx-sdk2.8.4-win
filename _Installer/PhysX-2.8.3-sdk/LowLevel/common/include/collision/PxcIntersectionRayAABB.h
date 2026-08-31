#ifndef PXC_INTERSECTIONRAYAABB_H
#define PXC_INTERSECTIONRAYAABB_H

#include "px_config.h"
#include "PxcVector.h"

// Collide ray defined by ray origin (rayOrigin) and ray direction (rayDirection)
// with the bounding box. Returns -1 on no collision and the face index
// for first intersection if a collision is found together with
// the distance to the collision points (tnear and tfar)
//
// ptchernev:
// Even though the above is the original comment by Pierre I am quite confident 
// that the tnear and tfar parameters are parameters along rayDirection of the
// intersection points:
//
// ip0 = rayOrigin + (rayDirection * tnear)
// ip1 = rayOrigin + (rayDirection * tfar)
//
// The return code is:
// -1 no intersection
//  0 the ray first hits the plane at aabbMin.x
//  1 the ray first hits the plane at aabbMin.y
//  2 the ray first hits the plane at aabbMin.z
//  3 the ray first hits the plane at aabbMax.x
//  4 the ray first hits the plane at aabbMax.y
//  5 the ray first hits the plane at aabbMax.z
//
// The return code will be -1 if the RAY does not intersect the AABB.
// The tnear and tfar values will give the parameters of the intersection 
// points between the INFINITE LINE and the AABB.

PxI32 PxcIntersectionRayAABB(const PxcVector& aabbMin, 
							 const PxcVector& aabbMax, 
							 const PxcVector& rayOrigin,
							 const PxcVector& rayDirection,
							 PxFloat& tnear,
							 PxFloat& tfar);

#endif
